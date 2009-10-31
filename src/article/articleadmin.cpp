// ライセンス: GPL2

//#define _DEBUG
#include "jddebug.h"

#include "articleadmin.h"
#include "articleviewbase.h"
#include "font.h"
#include "toolbar.h"
#include "toolbarsearch.h"

#include "dbtree/interface.h"

#include "jdlib/miscutil.h"
#include "jdlib/jdregex.h"
#include "jdlib/miscmsg.h"
#include "jdlib/timeout.h"

#include "skeleton/view.h"
#include "skeleton/dragnote.h"

#include "icons/iconmanager.h"

#include "history/historymanager.h"

#include "global.h"
#include "type.h"
#include "viewfactory.h"
#include "sharedbuffer.h"
#include "session.h"
#include "command.h"
#include "config/globalconf.h"
#include "dndmanager.h"

ARTICLE::ArticleAdmin *instance_articleadmin = NULL;

ARTICLE::ArticleAdmin* ARTICLE::get_admin()
{
    if( ! instance_articleadmin ) instance_articleadmin = new ARTICLE::ArticleAdmin( URL_ARTICLEADMIN );
    assert( instance_articleadmin );

    return instance_articleadmin;
}

void ARTICLE::delete_admin()
{
    if( instance_articleadmin ) delete instance_articleadmin;
    instance_articleadmin = NULL;
}


using namespace ARTICLE;

ArticleAdmin::ArticleAdmin( const std::string& url )
    : SKELETON::Admin( url ), m_toolbar( NULL ), m_toolbarsimple( NULL ), m_search_toolbar( NULL )
{
    set_use_viewhistory( true );
    ARTICLE::init_font();

    get_notebook()->set_dragable( true );
    get_notebook()->set_fixtab( false );
    if( ! SESSION::get_show_article_tab() ) get_notebook()->set_show_tabs( false );

    setup_menu();

    // スムーススクロール用タイマセット
    // オートスクロール時など、スムースにスクロールをするため描画用タイマーを
    // メインタイマと別にする。DrawAreaBase::clock_in_smooth_scroll() も参照すること
    sigc::slot< bool > slot_timeout = sigc::bind( sigc::mem_fun(*this, &ArticleAdmin::clock_in_smooth_scroll ), 0 );
    JDLIB::Timeout::connect( slot_timeout, TIMER_TIMEOUT_SMOOTH_SCROLL );
}


ArticleAdmin::~ArticleAdmin()
{
#ifdef _DEBUG    
    std::cout << "ArticleAdmin::~ArticleAdmin\n";
#endif

    if( m_toolbar ) delete m_toolbar;
    if( m_toolbarsimple ) delete m_toolbarsimple;
    if( m_search_toolbar ) delete m_search_toolbar;

    SESSION::set_article_URLs( get_URLs() );
    SESSION::set_article_locked( get_locked() );
    SESSION::set_article_page( get_current_page() );
    ARTICLE::init_font();
}


bool ArticleAdmin::clock_in_smooth_scroll( int timer_number )
{
    // アクティブなビューにクロックを送る
    ArticleViewBase* view = dynamic_cast< ArticleViewBase* >(  get_current_view() );
    if( view ) view->clock_in_smooth_scroll();

    return true;
}


// 前回開いていたURLを復元
void ArticleAdmin::restore( const bool only_locked )
{
#ifdef _DEBUG
    std::cout << "ArticleAdmin::restore\n";
#endif

    int set_page_num = 0;
    const bool online = SESSION::is_online();
    SESSION::set_online( false );

    const std::list< std::string >& list_url = SESSION::get_article_URLs();
    std::list< std::string >::const_iterator it_url = list_url.begin();

    const std::list< bool >& list_locked = SESSION::get_article_locked();
    std::list< bool >::const_iterator it_locked = list_locked.begin();

    for( int page = 0; it_url != list_url.end(); ++it_url, ++page ){

        // タブのロック状態
        bool lock = false;
        if( it_locked != list_locked.end() ){
            if( (*it_locked ) ) lock = true;
            ++it_locked;
        }

        // ロックされているものだけ表示
        if( only_locked && ! lock ) continue;

        // 板がDBに登録されていない場合は表示しない
        if( DBTREE::url_boardbase( *it_url ).empty() ){
            MISC::ERRMSG(  *it_url + " is not registered" );
            continue;
        }

        if( page == SESSION::article_page() ) set_page_num = get_tab_nums();

        // 検索ビューなど、復元したときに view のurlが変わることがあるので
        // view履歴内のURLを置き換える必要がある。そこで Admin::open_view() 中の
        // create_viewhistory()やappend_viewhistory()を実行しないで、ここで View履歴内のURLを更新する
        // Admin::Open_view()も参照すること
        const bool use_history = get_use_viewhistory();
        set_use_viewhistory( false );

        COMMAND_ARGS command_arg = url_to_openarg( *it_url, true, lock );
        if( ! command_arg.url.empty() ) open_view( command_arg );

        if( use_history ){

            // URLが変わっていたらview履歴内のURLを更新
            SKELETON::View* current_view = get_current_view();
            if( current_view && current_view->get_url() != *it_url ){
                HISTORY::get_history_manager()->replace_current_url_viewhistory( *it_url, current_view->get_url() );
                redraw_toolbar();
            }

            set_use_viewhistory( true );
        }
    }

    SESSION::set_online( online );
    if( get_tab_nums() ) set_command( "set_page", std::string(), MISC::itostr( set_page_num ) );
}


COMMAND_ARGS ArticleAdmin::url_to_openarg( const std::string& url, const bool tab, const bool lock )
{
    JDLIB::Regex regex;

    COMMAND_ARGS command_arg;
    command_arg.command = "open_view";
    command_arg.url = std::string();

#ifdef _DEBUG
    std::cout << "ArticleAdmin::url_to_openarg url = " << url << std::endl;
#endif    


    // レス抽出
    if( regex.exec( std::string( "(.*)" ) + ARTICLE_SIGN + RES_SIGN + "(.*)"
                    + CENTER_SIGN + "(.*)" + TIME_SIGN, url )){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        command_arg.arg4 = "RES";
        command_arg.arg5 = regex.str( 2 );
        if( regex.str( 3 ) != "0" ) command_arg.arg6 = regex.str( 3 );
    }

    // 名前抽出
    else if( regex.exec( std::string( "(.*)" ) + ARTICLE_SIGN + NAME_SIGN + "(.*)" + TIME_SIGN, url )){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        command_arg.arg4 = "NAME";
        command_arg.arg5 = regex.str( 2 );
    }

    // ID抽出
    else if( regex.exec( std::string( "(.*)" ) + ARTICLE_SIGN + ID_SIGN + "(.*)" + TIME_SIGN, url )){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        command_arg.arg4 = "ID";
        command_arg.arg5 = regex.str( 2 );
    }

    // ブックマーク抽出
    else if( regex.exec( std::string( "(.*)" ) + ARTICLE_SIGN + BOOKMK_SIGN, url )){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        command_arg.arg4 = "BM";
    }

    // 書き込み抽出
    else if( regex.exec( std::string( "(.*)" ) + ARTICLE_SIGN + POST_SIGN, url )){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        command_arg.arg4 = "POST";
    }

    // URL抽出
    else if( regex.exec( std::string( "(.*)" ) + ARTICLE_SIGN + URL_SIGN, url )){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        command_arg.arg4 = "URL";
    }

    // 参照
    else if( regex.exec( std::string( "(.*)" ) + ARTICLE_SIGN + REFER_SIGN + "(.*)" + TIME_SIGN, url )){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        command_arg.arg4 = "REF";
        command_arg.arg5 = regex.str( 2 );
    }

    // キーワード
    else if( regex.exec( std::string( "(.*)" ) + ARTICLE_SIGN + KEYWORD_SIGN + "(.*)"
                         + ORMODE_SIGN + "(.*)" + TIME_SIGN, url )){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        if( regex.str( 3 ) == "1" ) command_arg.arg4 = "KEYWORD_OR";
        else command_arg.arg4 = "KEYWORD";
        command_arg.arg5 = regex.str( 2 );
    }

    // キャッシュのログ検索
    else if( regex.exec( std::string( "(.*)" ) + BOARD_SIGN + KEYWORD_SIGN + "(.*)"
                         + ORMODE_SIGN + "(.*)" + TIME_SIGN, url )){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        if( command_arg.url == "allboard" ) command_arg.arg4 = "SEARCHALLLOG";
        else command_arg.arg4 = "SEARCHLOG";
        command_arg.arg5 = regex.str( 2 ); // query
        command_arg.arg6 = "noexec"; // Viewを開いた直後に検索を実行しない
    }

    // スレタイ検索
    else if( regex.exec( std::string( "(.*)" ) + TITLE_SIGN + KEYWORD_SIGN + "(.*)"
                         + ORMODE_SIGN + "(.*)" + TIME_SIGN, url )){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        command_arg.arg4 = "SEARCHTITLE";
        command_arg.arg5 = regex.str( 2 ); // query
        command_arg.arg6 = "noexec";  // Viewを開いた直後に検索を実行しない
    }

    // 書き込みログ表示
    else if( regex.exec( std::string( "(.*)" ) + POSTLOG_SIGN + "(.*)" + TIME_SIGN, url ) ){

        command_arg.url = regex.str( 1 );
        if( tab ) command_arg.arg1 = "true"; // タブで開く
        command_arg.arg2 = "true"; // 既に開いているかチェック無し
        if( lock ) command_arg.arg3 = "lock";

        command_arg.arg4 = "POSTLOG";
        command_arg.arg5 = regex.str( 2 ); // ログ番号
    }

    // MAIN
    else if( !url.empty() ){
        command_arg.url = url;
        if( tab ) command_arg.arg1 = "true";   // タブで開く
        command_arg.arg2 = "false";  // 既に開いているかチェック
        if( lock ) command_arg.arg3 = "lock";

        command_arg.arg4 = "MAIN";
    }

#ifdef _DEBUG
    std::cout << command_arg.url << std::endl
              << command_arg.arg1 << std::endl
              << command_arg.arg2 << std::endl
              << command_arg.arg3 << std::endl
              << command_arg.arg4 << std::endl
              << command_arg.arg5 << std::endl
              << command_arg.arg6 << std::endl << std::endl;
#endif

    return command_arg;
}



void ArticleAdmin::switch_admin()
{
    if( ! has_focus() ) CORE::core_set_command( "switch_article" );
}


//
// リストで与えられたページをタブで連続して開くとき(Admin::open_list())の引数セット
//
COMMAND_ARGS ArticleAdmin::get_open_list_args( const std::string& url, const COMMAND_ARGS& command_list )
{
    COMMAND_ARGS command_arg;
    command_arg.arg4 = "MAIN";

    return command_arg;
}


//
// カレントビューでポップアップ表示していたら隠す(インスタンスは削除しない)
//
void ArticleAdmin::hide_popup()
{
    SKELETON::View* view = get_current_view();
    if( view ) view->set_command( "hide_popup" );
}


//
// カレントビューでポップアップ表示していたら消す
//
void ArticleAdmin::delete_popup()
{
    SKELETON::View* view = get_current_view();
    if( view ) view->set_command( "delete_popup" );
}


//
// 全ポップアップを消す
//
void ArticleAdmin::delete_all_popups()
{
    std::list< SKELETON::View* > list_view = get_list_view();
    std::list< SKELETON::View* >::iterator it = list_view.begin();
    for( ; it != list_view.end(); ++it ){
        SKELETON::View* view = ( *it );
        if( view ) view->set_command( "delete_popup" );
    }
}


//
// view の作成
//
SKELETON::View* ArticleAdmin::create_view( const COMMAND_ARGS& command )
{
#ifdef _DEBUG    
    std::cout << "ArticleAdmin::create_view : " << command.arg4 << std::endl;
#endif

    delete_popup();

    int type = CORE::VIEW_NONE; 
    CORE::VIEWFACTORY_ARGS view_args;
    std::string str_jump;

    // メインビュー
    if( command.arg4 == "MAIN" ){
        type = CORE::VIEW_ARTICLEVIEW;
    }

    // キーワード抽出ビュー(AND)
    else if( command.arg4 == "KEYWORD" ){
        type = CORE::VIEW_ARTICLEDRAWOUT;
        view_args.arg1 = command.arg5;  // query
        view_args.arg2 = "AND";
    }

    // キーワード抽出ビュー(OR)
    else if( command.arg4 == "KEYWORD_OR" ){
        type = CORE::VIEW_ARTICLEDRAWOUT;
        view_args.arg1 = command.arg5; // query
        view_args.arg2 = "OR";
    }

    // レス抽出ビュー
    else if( command.arg4 == "RES" ){
        type = CORE::VIEW_ARTICLERES;
        view_args.arg1 = command.arg5; // レス番号 ( from-to )
        view_args.arg2 = "false";
        view_args.arg3 = command.arg6; // ジャンプ番号
        str_jump = command.arg6;
    }

    // 名前抽出ビュー
    else if( command.arg4 == "NAME" ){
        type = CORE::VIEW_ARTICLENAME;
        view_args.arg1 = command.arg5; // 名前
    }

    // ID 抽出ビュー
    else if( command.arg4 == "ID" ){
        type = CORE::VIEW_ARTICLEID;
        view_args.arg1 = command.arg5; // ID
    }

    // ブックマーク抽出ビュー
    else if( command.arg4 == "BM" ){
        type = CORE::VIEW_ARTICLEBM;
    }

    // 書き込み抽出ビュー
    else if( command.arg4 == "POST" ){
        type = CORE::VIEW_ARTICLEPOST;
    }

    // URL抽出ビュー
    else if( command.arg4 == "URL" ){
        type = CORE::VIEW_ARTICLEURL;
    }

    // 参照抽出ビュー
    else if( command.arg4 == "REF" ){
        type = CORE::VIEW_ARTICLEREFER;
        view_args.arg1 = command.arg5; // 対象レス番号
    }

    // ログ検索
    else if( command.arg4 == "SEARCHLOG" ){
        type = CORE::VIEW_ARTICLESEARCHLOG;
        view_args.arg1 = command.arg5;  // query
        view_args.arg2 = command.arg6;  // exec
    }

    // 全キャッシュログ検索
    else if( command.arg4 == "SEARCHALLLOG" ){
        type = CORE::VIEW_ARTICLESEARCHALLLOG;
        view_args.arg1 = command.arg5;  // query
        view_args.arg2 = command.arg6;  // exec
    }

    // スレタイ検索
    else if( command.arg4 == "SEARCHTITLE" ){
        type = CORE::VIEW_ARTICLESEARCHTITLE;
        view_args.arg1 = command.arg5;  // query
        view_args.arg2 = command.arg6;  // exec
    }

    // 書き込みログ表示
    else if( command.arg4 == "POSTLOG" ){
        type = CORE::VIEW_ARTICLEPOSTLOG;
        view_args.arg1 = command.arg5;  // ログ番号
    }

    else return NULL;

    SKELETON::View* view = CORE::ViewFactory( type, command.url, view_args );
    assert( view != NULL );

    // ジャンプ
    if( ! str_jump.empty() ){
#ifdef _DEBUG    
        std::cout << "goto " << str_jump << std::endl;
#endif
        set_command( "goto_num", view->get_url(), str_jump );
    }

    return view;
}


//
// ツールバー表示
//
void ArticleAdmin::show_toolbar()
{
    // まだ作成されていない場合は作成する
    if( ! m_toolbar ){

        // 通常のツールバー( TOOLBAR_ARTICLE )
        m_toolbar = new ArticleToolBar();
        get_notebook()->append_toolbar( *m_toolbar );

        // 簡易版ツールバー( TOOLBAR_SIMPLE )
        m_toolbarsimple = new ArticleToolBarSimple();
        get_notebook()->append_toolbar( *m_toolbarsimple );

        // ログ検索などのツールバー( TOOLBAR_SEARCH )
        m_search_toolbar = new SearchToolBar();
        get_notebook()->append_toolbar( *m_search_toolbar );

        if( SESSION::get_show_article_toolbar() ){
            m_toolbar->open_buttonbar();
            m_toolbarsimple->open_buttonbar();
            m_search_toolbar->open_buttonbar();
        }
    }

    get_notebook()->show_toolbar();
}


//
// ツールバー表示/非表示切り替え
//
void ArticleAdmin::toggle_toolbar()
{
    if( ! m_toolbar ) return;

    if( SESSION::get_show_article_toolbar() ){

        m_toolbar->open_buttonbar();
        m_toolbarsimple->open_buttonbar();
        m_search_toolbar->open_buttonbar();

        switch( get_notebook()->get_current_toolbar() ){

            case TOOLBAR_ARTICLE:
                m_toolbar->show_toolbar();
                break;

            case TOOLBAR_SIMPLE:
                m_toolbarsimple->show_toolbar();
                break;

            case TOOLBAR_SEARCH:
                m_search_toolbar->show_toolbar();
                break;
        }

    }
    else{
        m_toolbar->close_buttonbar();
        m_toolbarsimple->close_buttonbar();
        m_search_toolbar->close_buttonbar();
    }
}


//
// 検索バー表示
//
void ArticleAdmin::open_searchbar()
{
    if( ! m_toolbar ) return;

    SKELETON::View* view = get_current_view();
    if( ! view ) return;

    m_toolbar->open_searchbar();
    m_toolbarsimple->open_searchbar();
    m_search_toolbar->open_searchbar();

    switch( get_notebook()->get_current_toolbar() ){

        case TOOLBAR_ARTICLE:
            m_toolbar->show_toolbar();
            m_toolbar->focus_entry_search();
            break;

        case TOOLBAR_SIMPLE:
            m_toolbarsimple->show_toolbar();
            m_toolbarsimple->focus_entry_search();
            break;

        case TOOLBAR_SEARCH:
            m_search_toolbar->show_toolbar();
            m_search_toolbar->focus_entry_search();
            break;
    }
}


//
// 検索バー非表示
//
void ArticleAdmin::close_searchbar()
{
    if( ! m_toolbar ) return;

    m_toolbar->close_searchbar();
    m_toolbarsimple->close_searchbar();
    m_search_toolbar->close_searchbar();
}


//
// ローカルなコマンド
//
void ArticleAdmin::command_local( const COMMAND_ARGS& command )
{
    if( command.command == "goto_num" ){
        SKELETON::View* view = get_view( command.url );
        if( view ) view->set_command( "goto_num", command.arg1 );
    }

    // ポップアップを隠す(インスタンスは削除しない)
    else if( command.command == "hide_popup" ) hide_popup();

    // ポップアップを消去
    else if( command.command == "delete_popup" ) delete_popup();

    // 全ポップアップを消去
    else if( command.command == "delete_all_popups" ) delete_all_popups();

    // ポップアップメニューの再作成
    else if( command.command == "reset_popupmenu" ){

        std::list< SKELETON::View* > list_view = get_list_view();
        std::list< SKELETON::View* >::iterator it = list_view.begin();
        for( ; it != list_view.end(); ++it ){
            SKELETON::View* view = ( *it );
            if( view ) view->set_command( "reset_popupmenu" );
        }
    }

    // command.url を含むビューを全て再レイアウト
    else if( command.command == "relayout_views" ){

        std::list< SKELETON::View* > list_view = get_list_view( command.url );
        std::list< SKELETON::View* >::iterator it = list_view.begin();

        for( ; it != list_view.end(); ++it ){
            SKELETON::View* view = ( *it );
            if( view ) view->relayout();
        }
    }

    // フォント初期化
    else if( command.command == "init_font" ) ARTICLE::init_font();

    // ハイライト解除
    else if( command.command == "clear_highlight" ){
        SKELETON::View* view = get_view( command.url );
        if( view ) view->set_command( "clear_highlight" );
    }

    // 実況開始/停止
    else if( command.command == "live_start_stop" ){
        SKELETON::View* view = get_view( command.url );
        if( view ){

            view->set_command( "live_start_stop" );

            // ツールバー表示更新
            get_notebook()->set_current_toolbar( view->get_id_toolbar(), view );
        }
    }

    // 実況停止
    else if( command.command == "live_stop" ){
        SKELETON::View* view = get_view( command.url );
        if( view ){

            view->set_command( "live_stop" );

            // ツールバー表示更新
            get_notebook()->set_current_toolbar( view->get_id_toolbar(), view );
        }
    }
}


//
// タブをお気に入りにドロップした時にお気に入りがデータ送信を要求してきた
//
void ArticleAdmin::slot_drag_data_get( Gtk::SelectionData& selection_data, const int page )
{
#ifdef _DEBUG    
    std::cout << "ArticleAdmin::slot_drag_data_get page = " << page  << std::endl;
#endif

    SKELETON::View* view = ( SKELETON::View* )get_notebook()->get_nth_page( page );
    if( ! view ) return;

    const std::string url = view->get_url();
    
    CORE::DATA_INFO info;
    info.type = TYPE_THREAD;
    info.url = DBTREE::url_readcgi( url, 0, 0 );
    info.name = DBTREE::article_subject( info.url );
    info.data = std::string();
    info.path = Gtk::TreePath( "0" ).to_string();

    if( info.url.empty() ) return;

#ifdef _DEBUG    
    std::cout << "name = " << info.name << std::endl;
#endif

    CORE::DATA_INFO_LIST list_info;
    list_info.push_back( info );
    CORE::SBUF_set_list( list_info );

    selection_data.set( DNDTARGET_FAVORITE, get_url() );
}
