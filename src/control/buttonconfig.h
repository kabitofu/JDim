// ライセンス: GPL2
//
// マウスボタン設定
//

#ifndef _BUTTONCONFIG_H
#define _BUTTONCONFIG_H

#include "mousekeyconf.h"

namespace CONTROL
{
    class ButtonConfig : public MouseKeyConf
    {
      public:

        ButtonConfig();
        virtual ~ButtonConfig();

        virtual void load_conf();

        const bool is_toggled_tab_button(); // タブで開くボタンを入れ替えているか
        void toggle_tab_button( const bool toggle ); // タブで開くボタンを入れ替える

        const bool is_popup_warpmode(); // ポップアップ表示の時にクリックでワープするか
        void toggle_popup_warpmode(); // ポップアップ表示の時にクリックでワープする

      private:

        // ひとつの操作をデータベースに登録
        virtual void set_one_motion_impl( const int id, const int mode, const std::string& name, const std::string& str_motion );
    };
}

#endif
