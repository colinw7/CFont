#include <CFontMgr.h>

static CFontMgr *s_instance;

CFontMgr *
CFontMgr::
instance()
{
  if (! s_instance)
    s_instance = new CFontMgr;

  return s_instance;
}

void
CFontMgr::
release()
{
  delete s_instance;

  s_instance = nullptr;
}

CFontMgr::
CFontMgr()
{
}

void
CFontMgr::
setPrototype(CFontPtr ptr)
{
  prototype_ = ptr;

  font_list_.clear();
}

CFontPtr
CFontMgr::
lookupFont(const std::string &family, CFontStyle style, double size,
           double angle, double char_angle, uint x_res, uint y_res)
{
  auto p = font_list_.begin();

  for ( ; p != font_list_.end(); ++p)
    if ((*p)->getFamily    () == family           &&
        (*p)->getStyle     () == style            &&
        (*p)->getISize     () == uint(size)       &&
        (*p)->getIAngle    () == uint(angle)      &&
        (*p)->getICharAngle() == uint(char_angle) &&
        (*p)->getXRes      () == x_res            &&
        (*p)->getYRes      () == y_res)
      return *p;

  CFontPtr ptr;

  if (prototype_)
    ptr = prototype_->dup(family, style, size, angle, char_angle, int(x_res), int(y_res));
  else {
    if (getenv("CFONT_PROTO_DEBUG"))
      std::cerr << "Warning: no font prototype" << std::endl;

    auto *font = new CFont(family, style, size, angle, char_angle, int(x_res), int(y_res));

    ptr = CFontPtr(font);
  }

  addFont(ptr);

  return ptr;
}

CFontPtr
CFontMgr::
lookupFont(const std::string &full_name)
{
  auto p = font_list_.begin();

  for ( ; p != font_list_.end(); ++p)
    if ((*p)->getXFontName() == full_name)
      return *p;

  CFontPtr ptr;

  if (prototype_) {
    ptr = prototype_->dup(full_name);

    addFont(ptr);
  }

  return ptr;
}

bool
CFontMgr::
addFont(CFontPtr font)
{
  if (debug_)
    std::cerr << "Add Font: " << *font << std::endl;

  font_list_.push_back(font);

  return true;
}

bool
CFontMgr::
deleteFont(CFontPtr font)
{
  if (debug_)
    std::cerr << "Remove Font: " << *font << std::endl;

  font_list_.remove(font);

  return true;
}
