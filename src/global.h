// グローバルな定数などの定義

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <string>


// msec  内部クロックの周期
#define TIMER_TIMEOUT 50  

// 最大表示可能レス数
#define MAX_RESNUMBER 11000


// マウスジェスチャの最大ストローク
#define MAX_MG_LNG 5


// 画像アイコンの大きさ
#define ICON_SIZE 32 


// 書き込みビューのメール欄の空白
#define JD_MAIL_BLANK "jd_mail_blank"

//
// str をクリップボードにコピー
//
#define COPYCLIP( str ) do{  \
 Glib::RefPtr< Gtk::Clipboard > clip = Gtk::Clipboard::get(); \
 clip->set_text( str ); \
 clip = Gtk::Clipboard::get( GDK_SELECTION_PRIMARY ); \
 clip->set_text( str ); \
} while( false )


// SKELETON::SelectItemPrefの項目名
#define ITEM_NAME_MARK  "!"
#define ITEM_NAME_ID    "番号"
#define ITEM_NAME_NAME  "タイトル"
#define ITEM_NAME_RES   "レス"
#define ITEM_NAME_LOAD  "取得"
#define ITEM_NAME_NEW   "新着"
#define ITEM_NAME_SINCE "since"
#define ITEM_NAME_LASTWRITE "最終書込"
#define ITEM_NAME_SPEED "速度"

#define ITEM_NAME_WRITEMSG  "書き込み"
#define ITEM_NAME_OPENBOARD "板を開く"
#define ITEM_NAME_SEARCH    "検索"
#define ITEM_NAME_RELOAD    "再読み込み"
#define ITEM_NAME_STOPLOADING "読み込み中止"
#define ITEM_NAME_FAVORITE  "お気に入りに追加"
#define ITEM_NAME_DELETE    "削除"
#define ITEM_NAME_QUIT      "閉じる"

#define ITEM_NAME_NEWARTICLE   "新スレ作成"
#define ITEM_NAME_SEARCHBOX    "検索ボックス"
#define ITEM_NAME_SEARCH_NEXT  "次検索"
#define ITEM_NAME_SEARCH_PREV  "前検索"


// SESSION::get_item_*() の戻り値
enum
{
    ITEM_MARK = 0,
    ITEM_ID,
    ITEM_NAME,
    ITEM_RES,
    ITEM_LOAD,
    ITEM_NEW,
    ITEM_SINCE,
    ITEM_LASTWRITE,
    ITEM_SPEED,

    ITEM_WRITEMSG,
    ITEM_OPENBOARD,
    ITEM_SEARCH,
    ITEM_RELOAD,
    ITEM_STOPLOADING,
    ITEM_FAVORITE,
    ITEM_DELETE,
    ITEM_QUIT,

    ITEM_NEWARTICLE,
    ITEM_SEARCHBOX,
    ITEM_SEARCH_NEXT,
    ITEM_SEARCH_PREV,

    ITEM_END
};


// スレッド状態( or を取る)
enum
{
    STATUS_UNKNOWN = 0,  // 不明
    STATUS_NORMAL = 1,   // 通常
    STATUS_OLD = 2,      // DAT落ち or 板が移転した
    STATUS_BROKEN = 4,   // あぼーんなどで壊れている
    STATUS_UPDATE = 8   // 更新可能
};


// オートリロードのモード
enum
{
    AUTORELOAD_NOT = 0,
    AUTORELOAD_ONCE, // 1 回だけリロードして終わり
    AUTORELOAD_ON    // オートリロード実行中
};

// オートリロードの最小秒数
#define AUTORELOAD_MINSEC 10

// 更新チェックの最小秒数
#define CHECKUPDATE_MINSEC 300

// プロトコル
#define PROTO_ANCHORE "anc://"
#define PROTO_RES "res://"
#define PROTO_NAME "name://"
#define PROTO_ID "ID://"
#define PROTO_BE "BE://"
#define PROTO_ABONE "abone://"
#define PROTO_OR "or://"
#define PROTO_BROKEN "bloken://"


// 仮想 URL

#define URL_LOGIN2CH "jdlogin://login2ch"
#define URL_LOGINBE "jdlogin://loginbe"

#define URL_BBSLISTADMIN "jdadmin://bbslist"
#define URL_BBSLISTVIEW "jdview://bbslist"
#define URL_FAVORITEVIEW "jdview://favorite"

#define URL_BOARDADMIN "jdadmin://board"

#define URL_ARTICLEADMIN "jdadmin://article"

#define URL_IMAGEADMIN "jdadmin://image"

#define URL_MESSAGEADMIN "jdadmin://message"


// タイプ
enum 
{
    // 板のタイプ
    TYPE_BOARD_2CH = 0,
    TYPE_BOARD_2CH_COMPATI,  // 2ch 互換
    TYPE_BOARD_JBBS,         // したらば
    TYPE_BOARD_MACHI,        // まち
    TYPE_BOARD_UNKNOWN,

    // その他一般的なデータタイプ
    TYPE_BOARD,
    TYPE_THREAD,
    TYPE_THREAD_UPDATE,
    TYPE_THREAD_OLD,
    TYPE_IMAGE,
    TYPE_DIR,
    TYPE_DIR_END, // お気に入りの追加の時にサブディレクトリの終了の意味で使う
    TYPE_COMMENT,
    TYPE_LINK,
    TYPE_AA,

    TYPE_UNKNOWN
};



#endif
