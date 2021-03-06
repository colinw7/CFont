#ifndef CFONT_MGR_H
#define CFONT_MGR_H

#define CFontMgrInst CFontMgr::instance()

#include <CFont.h>
#include <list>

class CFontMgr {
 public:
  static CFontMgr *instance();

  static void release();

  CFontMgr();

 ~CFontMgr() { }

  void setPrototype(CFontPtr ptr);

  CFontPtr lookupFont(const std::string &family, CFontStyle style,
                      double size, double angle=0, double char_angle=0,
                      uint x_res=100, uint y_res=100);

  CFontPtr lookupFont(const std::string &full_name);

 private:
  bool addFont(CFontPtr font);
  bool deleteFont(CFontPtr font);

 private:
  typedef std::list<CFontPtr> FontList;

  FontList font_list_;
  CFontPtr prototype_;
  bool     debug_ { false };
};

#endif
