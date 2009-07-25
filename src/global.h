// グローバルな定数などの定義

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <string>


enum{

    TIMER_TIMEOUT = 50, // msec  内部クロックの周期
    TIMER_TIMEOUT_SMOOTH_SCROLL = 33, // msec  スレビューのスムーススクロール描画用クロック周期

    MAX_RESNUMBER = 11000, // 最大表示可能レス数

    MAX_MG_LNG = 5,  // マウスジェスチャの最大ストローク

    ICON_SIZE = 32 // 画像アイコンの大きさ
};


// 書き込みビューのメール欄の空白
#define JD_MAIL_BLANK "jd_mail_blank"


// SKELETON::SelectItemPrefの項目名
#define ITEM_NAME_BBSLISTVIEW "板一覧"
#define ITEM_NAME_FAVORITEVIEW "お気に入り"
#define ITEM_NAME_BOARDVIEW "スレ一覧"
#define ITEM_NAME_ARTICLEVIEW "スレビュー"
#define ITEM_NAME_IMAGEVIEW "画像ビュー"
#define ITEM_NAME_URL "URL"
#define ITEM_NAME_GO "移動"
#define ITEM_NAME_SEPARATOR "区切り"

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
#define ITEM_NAME_CHECK_UPDATE_ROOT "更新チェック"
#define ITEM_NAME_CHECK_UPDATE_OPEN_ROOT "更新チェックしてタブで開く"
#define ITEM_NAME_DELETE    "削除"
#define ITEM_NAME_QUIT      "閉じる"
#define ITEM_NAME_PREVVIEW  "前へ戻る"
#define ITEM_NAME_NEXTVIEW  "次へ進む"
#define ITEM_NAME_LOCK      "タブをロックする"
#define ITEM_NAME_LIVE      "実況開始／停止"

#define ITEM_NAME_NEWARTICLE   "新スレ作成"
#define ITEM_NAME_SEARCHBOX    "検索ボックス"
#define ITEM_NAME_SEARCH_NEXT  "次検索"
#define ITEM_NAME_SEARCH_PREV  "前検索"

#define ITEM_NAME_INSERTTEXT  "テキストファイル挿入"
#define ITEM_NAME_LOCK_MESSAGE "書き込み後に閉じない"
#define ITEM_NAME_PREVIEW     "プレビュー表示"

#define ITEM_NAME_UNDO        "元に戻す(Undo)"
#define ITEM_NAME_REDO        "やり直し(Redo)"


// SESSION::get_item_*() の戻り値
enum
{
    ITEM_BBSLISTVIEW = 0,
    ITEM_FAVORITEVIEW,
    ITEM_BOARDVIEW,
    ITEM_ARTICLEVIEW,
    ITEM_IMAGEVIEW,
    ITEM_URL,
    ITEM_GO,
    ITEM_SEPARATOR,

    ITEM_MARK,
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
    ITEM_CHECK_UPDATE_ROOT,
    ITEM_CHECK_UPDATE_OPEN_ROOT,
    ITEM_DELETE,
    ITEM_QUIT,
    ITEM_PREVVIEW,
    ITEM_NEXTVIEW,
    ITEM_LOCK,
    ITEM_LIVE,

    ITEM_NEWARTICLE,
    ITEM_SEARCHBOX,
    ITEM_SEARCH_NEXT,
    ITEM_SEARCH_PREV,

    ITEM_INSERTTEXT,
    ITEM_LOCK_MESSAGE,
    ITEM_PREVIEW,

    ITEM_UNDO,
    ITEM_REDO,

    ITEM_END
};


// 板やスレッドの状態
enum
{
    STATUS_UNKNOWN = 0,  // 不明
    STATUS_NORMAL = 1,   // 通常
    STATUS_OLD = 2,      // DAT落ち or 板が移転した
    STATUS_BROKEN = 4,   // あぼーんなどで壊れている
    STATUS_UPDATE = 8,   // 更新可能
    STATUS_UPDATED = 16,   // 更新済み
};


// オートリロードのモード
enum
{
    AUTORELOAD_NOT = 0,
    AUTORELOAD_ONCE, // 1 回だけリロードして終わり
    AUTORELOAD_ON    // オートリロード実行中
};

enum
{
    AUTORELOAD_MINSEC = 10, // オートリロードの最小秒数
    MIN_LIVE_RELOAD_SEC = AUTORELOAD_MINSEC, // 実況時の最小リロード間隔

    CHECKUPDATE_MINSEC = 300 // 更新チェックの最小秒数
};


// 実況スクロールモード
enum
{
    LIVE_SCRMODE_VARIABLE = 0,  // 速度可変、速度がしきい値を越えると行単位でスクロール
    LIVE_SCRMODE_STEADY, // 速度一定、遅れがしきい値を越えると行単位でスクロール

    LIVE_SCRMODE_NUM
};


// プロトコル
#define PROTO_ANCHORE "anc://"
#define PROTO_RES "res://"
#define PROTO_NAME "name://"
#define PROTO_ID "ID://"
#define PROTO_BE "BE://"
#define PROTO_ABONE "abone://"
#define PROTO_OR "or://"
#define PROTO_BROKEN "bloken://"
#define PROTO_POSTLOG "postlog://"
#define PROTO_SSSP "sssp://"


// 仮想 URL

#define URL_LOGIN2CH "jdlogin://login2ch"
#define URL_LOGINBE "jdlogin://loginbe"
#define URL_LOGINP2 "jdlogin://loginp2"

#define URL_BBSLISTADMIN "jdadmin://bbslist"
#define URL_BOARDADMIN "jdadmin://board"
#define URL_ARTICLEADMIN "jdadmin://article"
#define URL_IMAGEADMIN "jdadmin://image"
#define URL_MESSAGEADMIN "jdadmin://message"

#define URL_BBSLISTVIEW "jdview://bbslist"
#define URL_FAVORITEVIEW "jdview://favorite"
#define URL_HISTTHREADVIEW "jdview://histthread"
#define URL_HISTBOARDVIEW "jdview://histboard"
#define URL_HISTCLOSEVIEW "jdview://histclose"

#define URL_USRCMD "jdpref://usrcmd"
#define URL_LINKFILTER "jdpref://linkfilter"
#define URL_BROWSER "jdpref://browser"
#define URL_ABOUTCONFIG "jdpref://aboutconfig"
#define URL_PRIVACY "jdpref://privacy"

#define URL_BOARD_LOCAL "file:///local"

#endif
