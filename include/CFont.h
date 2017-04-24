#ifndef CFONT_H
#define CFONT_H

#include <CRefPtr.h>
#include <CFontStyle.h>
#include <CImageLib.h>
#include <sys/types.h>
#include <map>

#define CFONT_DEF_SIZE 14

class CPixelRenderer;

struct CFontDefKey {
  CFontStyle style { CFONT_STYLE_NORMAL };
  uint       size  { CFONT_DEF_SIZE };
  uint       angle { 0 };

  CFontDefKey(CFontStyle style1, uint size1, uint angle1) :
   style(style1), size(size1), angle(angle1) {
  }

  bool operator<(const CFontDefKey &key) const {
    if (style < key.style) return true;
    if (style > key.style) return false;

    if (size < key.size) return true;
    if (size > key.size) return false;

    if (angle < key.angle) return true;
    if (angle > key.angle) return false;

    return false;
  }
};

//---

struct CFontDef {
  CFontStyle style { CFONT_STYLE_NORMAL };
  double     size  { CFONT_DEF_SIZE };
  double     angle { 0 };
  uint       x_res { 100 };
  uint       y_res { 100 };

  CFontDef(CFontStyle style1=CFONT_STYLE_NORMAL, double size1=CFONT_DEF_SIZE,
           double angle1=0, uint x_res1=100, uint y_res1=100) :
   style(style1), size(size1), angle(angle1), x_res(x_res1), y_res(y_res1) {
  }

  CFontDef(const CFontDef &font_def) :
   style(font_def.style), size(font_def.size), angle(font_def.angle),
   x_res(font_def.x_res), y_res(font_def.y_res) {
  }
};

//---

class CFontFamily {
 public:
  static CFontFamily &lookup(const std::string &name);

  explicit CFontFamily(const std::string &name);
  CFontFamily(const CFontFamily &family);
 ~CFontFamily();

  CFontFamily &operator=(const CFontFamily &family);

  const std::string &getName() const { return name_; }

  CFontDef &lookupFontDef(CFontStyle style, uint size=CFONT_DEF_SIZE, uint angle=0);

 private:
  typedef std::map<std::string,CFontFamily *> FamilyMap;
  typedef std::map<CFontDefKey,CFontDef *>    FontDefMap;

  std::string name_;
  FontDefMap  font_defs_;
};

//---

class CFont;

typedef CRefPtr<CFont> CFontPtr;

//---

class CFont {
 protected:
  CFont(const std::string &family, CFontStyle style, double size,
        double angle=0, double char_angle=0, int x_res=100, int y_res=100);

  explicit CFont(const std::string &full_name);

 public:
  virtual ~CFont() { }

 protected:
  CFont(const CFont &font);

  CFont &operator=(const CFont &font);

  friend class CRefPtr<CFont>;

  static uint getNextId() {
    static uint id;

    return ++id;
  }

 public:
  virtual CFontPtr dup() const;
  virtual CFontPtr dup(const std::string &family, CFontStyle style, double size,
                       double angle=0, double char_angle=0, int x_res=100, int y_res=100) const;
  virtual CFontPtr dup(const std::string &full_name) const;

 public:
  virtual uint               getId    () const { return id_    ; }
  virtual const std::string &getFamily() const { return family_; }

  virtual CFontStyle getStyle     () const { return font_def_.style     ; }
  virtual double     getSize      () const { return font_def_.size      ; }
  virtual uint       getISize     () const { return uint(getSize())     ; }
  virtual double     getAngle     () const { return font_def_.angle     ; }
  virtual uint       getIAngle    () const { return uint(getAngle())    ; }
  virtual double     getCharAngle () const { return char_angle_         ; }
  virtual uint       getICharAngle() const { return uint(getCharAngle()); }
  virtual uint       getXRes      () const { return font_def_.x_res     ; }
  virtual uint       getYRes      () const { return font_def_.y_res     ; }

 protected:
  virtual void setSize(double size) { font_def_.size = size; }

 public:
  CFontPtr rotated(double dangle=90) const;

 public:
  virtual double getCharWidth   () const { return getSize(); }
  virtual uint   getICharWidth  () const { return uint(getCharWidth()); }
  virtual double getCharAscent  () const { return getSize(); }
  virtual uint   getICharAscent () const { return uint(getCharAscent()); }
  virtual double getCharDescent () const { return 0; }
  virtual uint   getICharDescent() const { return uint(getCharDescent()); }
  virtual double getCharHeight  () const { return getCharAscent() + getCharDescent(); }
  virtual uint   getICharHeight () const { return uint(getCharHeight()); }

  virtual double getStringWidth (const std::string &str) const { return str.size()*getCharWidth(); }
  virtual uint   getIStringWidth(const std::string &str) const { return uint(getStringWidth(str)); }

  virtual void getStringBBox (const std::string &str, double x[4], double y[4]) const;
  virtual void getIStringBBox(const std::string &str, int    x[4], int    y[4]) const;

  virtual bool isProportional() const { return false; }

  virtual bool isBold  () const { return (getStyle() & CFONT_STYLE_BOLD  ); }
  virtual bool isItalic() const { return (getStyle() & CFONT_STYLE_ITALIC); }

  virtual bool isUnderline() const { return (getStyle() & CFONT_STYLE_UNDERLINE); }
  virtual bool isStrikeout() const { return (getStyle() & CFONT_STYLE_STRIKEOUT); }
  virtual bool isOverline () const { return (getStyle() & CFONT_STYLE_OVERLINE ); }

  virtual bool isSubscript  () const { return (getStyle() & CFONT_STYLE_SUBSCRIPT  ); }
  virtual bool isSuperscript() const { return (getStyle() & CFONT_STYLE_SUPERSCRIPT); }

  virtual double getCharAspect() const { return (getCharWidth()/getCharHeight()); }

  virtual std::string getXFontName() const;

  virtual CImagePtr getStringImage(const std::string &) {
    std::cerr << "undefined: CFont::getStringImage" << std::endl;
    return CImagePtr();
  }

  virtual CImagePtr getCharImage(char c) {
    std::string str(&c, 1);

    return getStringImage(str);
  }

  void print(std::ostream &os) const {
    os << getFamily() << ":" << fontStyleToString(getStyle()) << ":" <<
          getSize() << ":" << getAngle() << ":" << getCharAngle() << ":" <<
          getXRes() << ":" << getYRes();
  }

  friend std::ostream &operator<<(std::ostream &os, const CFont &font) {
    font.print(os);

    return os;
  }

  static bool decodeXFontName(const std::string &x_font_name, std::string &family,
                              CFontStyle &style, uint &size, uint &x_res, uint &y_res);

  static std::string encodeXFontName(const std::string &family, CFontStyle style, uint size);

  std::string encodeFontName();

  static std::string encodeFontName(const std::string &family, CFontStyle style, uint size);

  static bool decodeFontName(const std::string &name, std::string &family,
                             CFontStyle &style, uint &size);

 private:
  std::string buildXFontName() const;

 public:
  static CFontStyle  stringToFontStyle(const std::string &style);
  static std::string fontStyleToString(CFontStyle style);

 private:
  friend class CFontMgr;

  uint        id_ { 0 };
  std::string family_;
  CFontDef    font_def_;
  double      char_angle_ { 0 };
  std::string x_font_name_;
};

class CFontSet {
 public:
  CFontSet(const std::string &family="courier", uint size=12);

  void setup(const std::string &family, uint size);

  const std::string &getFamily() const { return family_; }

  uint getSize() const { return size_; }

  CFontPtr getFont(CFontStyle style) const;

 private:
  std::string family_ { "courier" };
  uint        size_   { 12 };
  CFontPtr    normal_;
  CFontPtr    bold_;
  CFontPtr    italic_;
  CFontPtr    boldi_;
};

#endif
