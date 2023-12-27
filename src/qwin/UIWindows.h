/*
 * Copyright (c) 2010 Jeffrey S. Larson  <jeff@circularlabs.com>
 * All rights reserved.
 * See the LICENSE file for the full copyright and license declaration.
 * 
 * ---------------------------------------------------------------------
 * 
 * Implementations of ComponentUI for the Windows platform.
 *
 * These implementations rely on multiple inheritance because there are two
 * class hierarchies:
 *  - The concrete/native class for shared Windows-specific functionality
 *  - The interface hierarchy of ComponentUI
 *
 * We have problems with multiple inheritance because all Windows implementations
 * inherit from ComponentUI on two paths:
 *  1. From the Windows-native hierarchy (WindowsComponent inherits ComponentUI)
 *  2. From the *UI interface (PanelUI inherits ComponentUI)
 *
 * It may be possible to implement the Windows native components without this use
 * of multiple inheritance, but, at a minimum, we can avoid the use of proxy
 * classes by leveraging virtual inheritance on the Interface side.
 * All *UI Interface classes inherit from ComponentUI virtually.
 * Thus, all Windows-implementation base classes that inherit ComponentUI should also
 * do so virtually, e.g.:
 *
 * class WindowsComponent : public NativeComponent, public virtual ComponentUI
 *                                 ^ concrete base         ^ virtual base interface
 *
 * And all derived components can inherit from WindowsComponent and their relevant
 * *UI interface normally without ambiguous/duplicate inheritance of ComponentUI, e.g.:
 *
 * class WindowsStatic : public WindowsComponent, public StaticUI
 */

#ifndef UI_WINDOWS_H
#define UI_WINDOWS_H

#include <windows.h>

#include "UIManager.h"

/****************************************************************************
 *                                                                          *
 *   								COLOR                                   *
 *                                                                          *
 ****************************************************************************/

#define MAX_PEN_WIDTH 4

class WindowsColor : public NativeColor {

  public:

	WindowsColor(Color* c);
	~WindowsColor();

	void setRgb(int rgb);
    HBRUSH getBrush();
    HPEN getPen();
    HPEN getPen(int width);

  private:

	Color* mColor;
	HBRUSH mBrush;
	HPEN mPens[MAX_PEN_WIDTH];
};

/****************************************************************************
 *                                                                          *
 *   								 FONT                                   *
 *                                                                          *
 ****************************************************************************/

class WindowsFont : public NativeFont {

  public:

	WindowsFont(Font* f);
	~WindowsFont();

	int getAscent();
	int getHeight();

    // is this used?
   	HFONT getHandle();

    // used by WindowsGraphics
	HFONT getHandle(HDC dc);

  private:

	Font* mFont;
	HFONT mHandle;
    TEXTMETRIC mTextMetric;

};

/****************************************************************************
 *                                                                          *
 *                                   TIMER                                  *
 *                                                                          *
 ****************************************************************************/

class WindowsTimer : public NativeTimer {

  public:

	WindowsTimer(SimpleTimer* t);
	~WindowsTimer();

    int getId();

    static SimpleTimer* getTimer(int id);

  private:

	SimpleTimer* mTimer;
    int mId;
};

/****************************************************************************
 *                                                                          *
 *   							 TEXT METRICS                               *
 *                                                                          *
 ****************************************************************************/

class WindowsTextMetrics : public TextMetrics {

  public:
	
	WindowsTextMetrics();
	void init(HDC dc);

	// TextMetrics interface methods

	int getHeight();
	int getMaxWidth();
	int getAverageWidth();
	int getAscent();
	int getExternalLeading();

  private:

    TEXTMETRIC mHandle;

};

/****************************************************************************
 *                                                                          *
 *   							   GRAPHICS                                 *
 *                                                                          *
 ****************************************************************************/

class WindowsGraphics : public Graphics {

    friend class WindowsWindow;

  public:

	WindowsGraphics();
	WindowsGraphics(HDC dc);
	~WindowsGraphics();

    Color* getColor();

	void save();
	void restore();
	void setColor(Color* c);
	void setBrush(Color* c);
	void setPen(Color* c);
	void setFont(Font* f);
	void setBackgroundColor(Color* c);
    void setXORMode(Color* c);
    void setXORMode();

	void drawString(const char* str, int x, int y);
	void drawLine(int x1, int y1, int x2, int y2);
    void drawRect(int x, int y, int width, int height);
    void drawRoundRect(int x, int y, int width, int height,
                       int arcWidth, int arcHeight);
    void drawOval(int x, int y, int width, int height);

    void fillRect(int x, int y, int width, int height);
    void fillRoundRect(int x, int y, int width, int height,
                       int arcWidth, int arcHeight);
    void fillOval(int x, int y, int width, int height);

    void fillArc(int x, int y, int width, int height,
                 int startAngle, int arcAngle);

	// extensions

    TextMetrics* getTextMetrics();
	void getTextSize(const char *text, Dimension* d);
	void getTextSize(const char *text, Font* font, Dimension* d);

    // used by Button and ListBox for "ownerdraw" 
    LPDRAWITEMSTRUCT getDrawItem();

  protected:

    void setDeviceContext(HDC dc);
    void setDrawItem(LPDRAWITEMSTRUCT di);

  private:

	void init();
    void startHollowShape();
    void endHollowShape();
    void getRadial(int centerx, int centery, int radius, int angle,
                   int* radialx, int* radialy);


	HDC mHandle;
	HFONT mDefaultFont;
    HBRUSH mHollowBrush;
    HBRUSH mSaveBrush;
	WindowsTextMetrics mTextMetrics;

	// when created in response to a WM_DRAWITEM message
	LPDRAWITEMSTRUCT mDrawItem;

    Color* mColor;
    Color* mBackground;
    Font* mFont;
};

/****************************************************************************
 *                                                                          *
 *   							SYSTEM DIALOGS                              *
 *                                                                          *
 ****************************************************************************/

class WindowsOpenDialog : public SystemDialogUI {

  public:

	WindowsOpenDialog(OpenDialog* od);
	~WindowsOpenDialog();

	void show();

  private:
    
    void getWindowsFilter(const char* src, char* dest);
    void getExtension(const char* filter, int index, char* extension);

    OpenDialog* mDialog;
};

class WindowsColorDialog : public SystemDialogUI {

  public:

	WindowsColorDialog(ColorDialog* cd);
	~WindowsColorDialog();

	void show();

  private:
    
    ColorDialog* mDialog;

};

class WindowsMessageDialog : public SystemDialogUI {

  public:

	WindowsMessageDialog(MessageDialog* cd);
	~WindowsMessageDialog();

	void show();

  private:
    
    MessageDialog* mDialog;
};

/****************************************************************************
 *                                                                          *
 *                            WINDOWS COMPONENT UI                          *
 *                                                                          *
 ****************************************************************************/

/**
 * The base class for all Windows implementations of ComponentUI.
 * Unfortunately have to get to these via proxy due to odd 
 * MSVC/C++ behavior with multiple inheritance.
 */
class WindowsComponent : public NativeComponent, public virtual ComponentUI {

  public:

	WindowsComponent();
    virtual ~WindowsComponent();
    void subclassWindowProc();

    static HWND getHandle(Component* c);
    static HWND getWindowHandle(Component* c);
	
	// this is defined by the NativeComponent interface so it must 
	// return void*
    void* getHandle();

    virtual Component* getComponent() = 0;
    HWND getParentHandle();
	int getWindowStyle();
    WindowsContext* getWindowsContext(Component* c);
    void detach();
	virtual void updateNativeBounds(Bounds* b);
    bool isOpen();

	virtual void command(int code);
	virtual void notify(int code);

    virtual LONG_PTR messageHandler(UINT msg, WPARAM wparam, LPARAM lparam);

    // default implementations for ComponentUI methods

    virtual void paint(Graphics* g);
	virtual Color* colorHook(Graphics* g);
    virtual void invalidate(Component* c);
    virtual void updateBounds();
    virtual void close();
    virtual void invalidateHandle();
    virtual void setEnabled(bool b);
    virtual bool isEnabled();
    virtual void setVisible(bool b);
    virtual bool isVisible();
    virtual void setFocus();
	virtual void debug();

  protected:

    HWND mHandle;
    WNDPROC mWindowProc;

};

/****************************************************************************
 *                                                                          *
 *   								STATIC                                  *
 *                                                                          *
 ****************************************************************************/

class WindowsStatic : public WindowsComponent, public StaticUI {

  public:
	
	WindowsStatic(Static* s);
	~WindowsStatic();

    Component* getComponent() {
        return mStatic;
    }

	void setText(const char* s);
	void setBitmap(const char* s);
	void setIcon(const char* s);
	void getPreferredSize(Window* w, Dimension* d);

	void open();
	Color* colorHook(Graphics* g);

    void postOpen() {
    }
    bool isNativeParent() {
        // on Windows statics are always parents
        return true;
    }
  private:

	Static* mStatic;
    bool mAutoColor;

};

/****************************************************************************
 *                                                                          *
 *                                   PANEL                                  *
 *                                                                          *
 ****************************************************************************/

class WindowsPanel : public WindowsComponent, public PanelUI {

  public:
	
	WindowsPanel(Panel* p);
	~WindowsPanel();

    Component* getComponent() {
        return mPanel;
    }

    bool isNativeParent();
	void open();
    void postOpen();
	Color* colorHook(Graphics* g);

    void getPreferredSize(Window* w, Dimension* d) {
        // defined by children
    }
  private:

	Panel* mPanel;

};

/****************************************************************************
 *                                                                          *
 *   								BUTTON                                  *
 *                                                                          *
 ****************************************************************************/

class WindowsButton : public WindowsComponent, public ButtonUI {

  public:

	WindowsButton(Button * b);
	~WindowsButton();

    Component* getComponent() {
        return mButton;
    }

    void setText(const char* text);
	void click();
	void getPreferredSize(Window* w, Dimension* d);
	void open();

	Color* colorHook(Graphics* g);
	void command(int code);
	void updateBounds();

	void paint(Graphics* g);

    void postOpen() {
    }
    bool isNativeParent() {
        return false;
    }
  private:

	Button* mButton;

};

/****************************************************************************
 *                                                                          *
 *   							 RADIO BUTTON                               *
 *                                                                          *
 ****************************************************************************/

class WindowsRadioButton : public WindowsComponent, public RadioButtonUI {

  public:

	WindowsRadioButton();
	WindowsRadioButton(RadioButton *b);
	~WindowsRadioButton();

    Component* getComponent() {
        return mButton;
    }

	void setSelected(bool b);
	bool isSelected();

	void open();
	void getPreferredSize(Window* w, Dimension* d);
	void command(int code);

    void postOpen() {
    }
    bool isNativeParent() {
        return false;
    }
    void setText(const char* text) {
    }
    void click() {
    }
  private:

	RadioButton* mButton;

};

//////////////////////////////////////////////////////////////////////
//
// Radios
//
//////////////////////////////////////////////////////////////////////

class WindowsRadios : public WindowsComponent, public RadiosUI {

  public:

	WindowsRadios();
	WindowsRadios(Radios *r);
	~WindowsRadios();

    Component* getComponent() {
        return mRadios;
    }

	void open();
	void changeSelection(RadioButton* b);

    void postOpen() {
    }
    void getPreferredSize(Window* w, Dimension* d) {
        // defined by children
    }
    bool isNativeParent() {
        return false;
    }
    bool isOpen() {
        return true;
    }
    void command(int code) {
    }
    Color* colorHook(Graphics* g) {
        return NULL;
    }
    void invalidate(Component* c) {
    }
    void paint(Graphics* g) {
    }
    void close() {
    }
    void invalidateHandle() {
    }
    void updateBounds() {
    }
    void setEnabled(bool b) {
    }
    bool isEnabled() {
        return false;
    }
    void setVisible(bool b) {
    }
    bool isVisible() {
        return false;
    }
    void setFocus() {
    }
  private:

	Radios* mRadios;

};

/****************************************************************************
 *                                                                          *
 *   							   CHECKBOX                                 *
 *                                                                          *
 ****************************************************************************/

class WindowsCheckbox : public WindowsComponent, public CheckboxUI {

  public:

	WindowsCheckbox(Checkbox *cb);
	~WindowsCheckbox();

    Component* getComponent() {
        return mCheckbox;
    }

	void setSelected(bool b);
	bool isSelected();
    void getPreferredSize(Window* w, Dimension* d);
	void open();
	void command(int code);

    void postOpen() {
    }
    bool isNativeParent() {
        return false;
    }

    void setText(const char* text) {
    }
    void click() {
    }

  private:

	Checkbox* mCheckbox;

};

/****************************************************************************
 *                                                                          *
 *   							  COMBO BOX                                 *
 *                                                                          *
 ****************************************************************************/

class WindowsComboBox : public WindowsComponent, public ComboBoxUI {

  public:

	WindowsComboBox(ComboBox* cb);
	~WindowsComboBox();

    Component* getComponent() {
        return mComboBox;
    }

	void setValues(StringList* values);
	void addValue(const char* value);
	void setSelectedIndex(int i);
	void setSelectedValue(const char* value);
	int getSelectedIndex();
	char* getSelectedValue();

    void open();
	void getPreferredSize(Window* w, Dimension* d);
    void command(int code);

    // we have an unusal bounds updater
    void updateBounds();

    void postOpen() {
    }
    bool isNativeParent() {
        return false;
    }
  private:

    int getFullHeight();

	ComboBox* mComboBox;

};

/****************************************************************************
 *                                                                          *
 *   							   LIST BOX                                 *
 *                                                                          *
 ****************************************************************************/

class WindowsListBox : public WindowsComponent, public ListBoxUI {

  public:

	WindowsListBox(ListBox* cb);
	~WindowsListBox();

    Component* getComponent() {
        return mListBox;
    }

	void setValues(StringList* values);
	void addValue(const char* value);
	void setAnnotations(StringList* values);

	void setSelectedIndex(int i);
	int getSelectedIndex();
	bool isSelected(int i);

	void open();
    void getPreferredSize(Window* w, Dimension* d);
	void command(int code);

    // ownerdraw support
	void paint(Graphics* g);

    void postOpen() {
    }
    bool isNativeParent() {
        return false;
    }
  private:

	ListBox* mListBox;

};

/****************************************************************************
 *                                                                          *
 *   							  GROUP BOX                                 *
 *                                                                          *
 ****************************************************************************/

class WindowsGroupBox : public WindowsComponent, public GroupBoxUI {

  public:

	WindowsGroupBox(GroupBox* gb);
	~WindowsGroupBox();

    Component* getComponent() {
        return mGroupBox;
    }

	void setText(const char* s);
	void open();

    void postOpen() {
    }
    void getPreferredSize(Window* w, Dimension* d) {
        // defined by GroupBox but should be in the UI
    }
    bool isNativeParent() {
        return false;
    }
  private:

	GroupBox* mGroupBox;

};

/****************************************************************************
 *                                                                          *
 *                                    TEXT                                  *
 *                                                                          *
 ****************************************************************************/

class WindowsText : public WindowsComponent, public virtual TextUI {

  public:

    WindowsText(Text* t);
    ~WindowsText();

    Component* getComponent() {
        return mText;
    }

	void setEditable(bool b);
    void setText(const char* s);
    char* getText();

    virtual void open();
    virtual void getPreferredSize(Window* w, Dimension* d);

    void command(int code);
    LONG_PTR messageHandler(UINT msg, WPARAM wparam, LPARAM lparam);

    void postOpen() {
    }
    bool isNativeParent() {
        return false;
    }
  protected:

    Text* mText;

};

/****************************************************************************
 *                                                                          *
 *                                 TEXT AREA                                *
 *                                                                          *
 ****************************************************************************/

class WindowsTextArea : public WindowsText, public TextAreaUI {
  public:

    WindowsTextArea(TextArea* t);
    ~WindowsTextArea();

    void open();
	void getPreferredSize(Window* w, Dimension* d);

    void postOpen() {
    }
    bool isNativeParent() {
        return false;
    }
  private:

};

/****************************************************************************
 *                                                                          *
 *   							   TOOL BAR                                 *
 *                                                                          *
 ****************************************************************************/

class WindowsToolBar : public WindowsComponent, public ToolBarUI {

  public:

	WindowsToolBar(ToolBar* tb);
	~WindowsToolBar();

    Component* getComponent() {
        return mToolBar;
    }

	void open();
    void postOpen() {
    }
    void getPreferredSize(Window* w, Dimension* d) {
        // not implemented yet
    }
    bool isNativeParent() {
        return false;
    }
   private:

	ToolBar* mToolBar;

};

/****************************************************************************
 *                                                                          *
 *   							  STATUS BAR                                *
 *                                                                          *
 ****************************************************************************/

class WindowsStatusBar : public WindowsComponent, public StatusBarUI {

  public:

	WindowsStatusBar(StatusBar* sb);
	~WindowsStatusBar();

    Component* getComponent() {
        return mStatusBar;
    }

	void open();

    void postOpen() {
    }
    void getPreferredSize(Window* w, Dimension* d) {
        // not implemented yet
    }
    bool isNativeParent() {
        return false;
    }

  private:

	StatusBar* mStatusBar;

};

/****************************************************************************
 *                                                                          *
 *   							 TABBED PANE                                *
 *                                                                          *
 ****************************************************************************/

class WindowsTabbedPane : public WindowsComponent, public TabbedPaneUI {

  public:
	
	WindowsTabbedPane(TabbedPane* sb);
	~WindowsTabbedPane();

    Component* getComponent() {
        return mTabbedPane;
    }

	void setSelectedIndex(int i);
    int getSelectedIndex();

	void open();
	void getPreferredSize(Window* w, Dimension* d);
    void command(int code);
    void notify(int code);

    void postOpen() {
    }
    bool isNativeParent() {
        // This feels like it should be true but apparently not
        // we have historically just overlayed a child window on top.
        // I tried setting it true to debug the problem with lightweight
        // labels but that didn't work.
        return false;
    }
   private:

    void forceHeavyLabels(Component* c);

	TabbedPane* mTabbedPane;

};

/****************************************************************************
 *                                                                          *
 *                                   TABLE                                  *
 *                                                                          *
 ****************************************************************************/

class WindowsTable : public WindowsComponent, public TableUI {

  public:

	WindowsTable(Table* cb);
	~WindowsTable();

    Component* getComponent() {
        return mTable;
    }

    void updateBounds();
    void rebuild();
	void setSelectedIndex(int i);
	int getSelectedIndex();
	bool isSelected(int i);

	void open();
    void getPreferredSize(Window* w, Dimension* d);
	void command(int code);
    List* getColumnWidths(Window* w);

    // ownerdraw support
	void paint(Graphics* g);

    void postOpen() {
    }
    bool isNativeParent() {
        return false;
    }
  private:

	int getMaxColumnWidth(Window* w, TableModel* model, int col);

	Table* mTable;
    List* mColumnWidths;
	Font* mDefaultColumnFont;
	Font* mDefaultCellFont;
	int mHeaderHeight;
};

/****************************************************************************
 *                                                                          *
 *   								 TREE                                   *
 *                                                                          *
 ****************************************************************************/

class WindowsTree : public WindowsComponent, public TreeUI {

  public:

	WindowsTree(Tree* t);
	~WindowsTree();

    Component* getComponent() {
        return mTree;
    }

	void open();
    void postOpen() {
    }
    void getPreferredSize(Window* w, Dimension* d) {
        // not implemented yet
    }
    bool isNativeParent() {
        return false;
    }

   private:

	Tree* mTree;

};

/****************************************************************************
 *                                                                          *
 *   							  SCROLL BAR                                *
 *                                                                          *
 ****************************************************************************/

class WindowsScrollBar : public WindowsComponent, public ScrollBarUI {

  public:

	WindowsScrollBar(ScrollBar* sb);
	~WindowsScrollBar();

    Component* getComponent() {
        return mScrollBar;
    }

	void open();
	void getPreferredSize(Window* w, Dimension* d);
	void update();

	void scroll(int code);
	Color* colorHook(Graphics* g);
    void postOpen() {
    }
    bool isNativeParent() {
        return false;
    }

  private:

	ScrollBar* mScrollBar;

};

/****************************************************************************
 *                                                                          *
 *   								WINDOW                                  *
 *                                                                          *
 ****************************************************************************/

class WindowsWindow : public WindowsComponent, public virtual WindowUI {

  public:
	
    WindowsWindow(Window* win);
    ~WindowsWindow();

    Component* getComponent() {
        return mWindow;
    }

    Graphics* getGraphics();
	virtual LONG_PTR messageHandler(UINT msg, WPARAM wparam, LPARAM lparam);

    void updateNativeBounds(Bounds* b);
	bool isChild();
	virtual void open();
	void close();
	int run();
	void relayout();
	void toFront();

    void setBackground(Color* c);

    // Temorary transition support

    WindowsContext* getContext();
	HINSTANCE getInstance();

	// has to be public so we can tear down under
	// special cirumstances (VST DLL unloading)

    void postOpen() {
    }
    void getPreferredSize(Window* w, Dimension* d) {
        // size defined by children
    }
    bool isNativeParent() {
        return true;
    }

  protected:

	/**
	 * True if we've registered window classes.
	 */
	static bool ClassesRegistered;
	void registerClasses(const char* icon);

	void menuHandler(int id);

	void captureNativeBounds(bool warn);
	void center();

	virtual HWND getParentWindowHandle();

    void setupToolTips();
    void setupToolTips(Component* c);
    void mouseHandler(int msg, int keys, int x, int y);
    void keyHandler(int msg, int key, long status);
    Menu* getMenu(HMENU handle);

    Window* mWindow;
	HACCEL mAccel;
    HWND mToolTip;

	// This one has the cannonical HDC for use outside of message handlers
	HDC mDeviceContext;
    WindowsGraphics* mGraphics;

	// This one is initialized by message handlers and has a transient HDC
	WindowsGraphics* mEventGraphics;

    // event cache
	WindowEvent* mWindowEvent;
    MouseEvent* mMouseEvent;
    KeyEvent* mKeyEvent;

    // the component receiving mouseDragged events
    Component* mDragComponent;

	// true if this is a child window owned by a window out of
	// our control
	bool mChild;

    // kludge necessary to get the origin of the damn client region
    // since GetClientRect sucks
    int mFuckingClientTopOffset;
    int mFuckingClientLeftOffset;

  private:

};

//////////////////////////////////////////////////////////////////////
//
// WindowsHostFrame
//
//////////////////////////////////////////////////////////////////////

class WindowsHostFrame : public WindowsWindow, public HostFrameUI {

  public:
	
    WindowsHostFrame(HostFrame* f);
    ~WindowsHostFrame();

	void open();
    void close();

    void postOpen() {
    }
    void getPreferredSize(Window* w, Dimension* d) {
        // size defined by children
    }
    bool isNativeParent() {
        return true;
    }

    // overload from WindowsWindow to return the host window handle
    HWND getParentWindowHandle();

  protected:

  private:

};

/****************************************************************************
 *                                                                          *
 *                                   DIALOG                                 *
 *                                                                          *
 ****************************************************************************/

class WindowsDialog : public WindowsWindow, public DialogUI {

  public:
	
    WindowsDialog(Dialog* d);
    ~WindowsDialog();

    LRESULT dialogHandler(UINT msg, WPARAM wparam, LPARAM lparam);
    void show();

    void postOpen() {
    }
    void getPreferredSize(Window* w, Dimension* d) {
        // size defined by children
    }
    bool isNativeParent() {
        return true;
    }

  protected:

    unsigned long modalEventLoop();

};

/****************************************************************************
 *                                                                          *
 *                                    MENU                                  *
 *                                                                          *
 ****************************************************************************/

/**
 * The same UI class is used for all MenuItems.
 */
class WindowsMenuItem : public WindowsComponent, public MenuUI {

  public: 

    WindowsMenuItem(MenuItem* item);
    ~WindowsMenuItem();

    Component* getComponent() {
        return mItem;
    }

    HMENU getMenuHandle();

	void setChecked(bool b);
	void setEnabled(bool b);
	void removeAll();

    void open();
    void close();
    void invalidateHandle();
    void openPopup(Window* window, int x, int y);
    void postOpen() {
    }
    void getPreferredSize(Window* w, Dimension* d) {
        // not an embedded component
    }
    bool isNativeParent() {
        return false;
    }

	// for use by WindowsWindowUI?
    Menu* findMenu(HMENU handle);

  private:

    void setNativeState(int mask);
    HMENU openResourceMenu(const char* resource);
    // hey!! we have one of these inherited from WindowsComponent
    // which isn't marked virtual and differs only in return type 
    // (which is probably void*) anyway
    HMENU getParentHandle();
    void openPopupMenu();
    void openMenuBar();
    void openMenu();
    void openItem();


    MenuItem* mItem;
    HMENU mMenuHandle;
    bool mCreated;

};

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
#endif
