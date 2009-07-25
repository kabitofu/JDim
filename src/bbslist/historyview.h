// ライセンス: GPL2

// 履歴ビュー

#ifndef _HISTORYVIEW_H
#define _HISTORYVIEW_H

#include "bbslistviewbase.h"

namespace BBSLIST
{
    class HistoryViewBase : public BBSListViewBase
    {
        std::string m_file_xml;

      public:

        HistoryViewBase( const std::string& url, const std::string& file_xml,
                         const std::string& arg1, const std::string& arg2 );
        virtual ~HistoryViewBase();

        virtual void show_view();

      protected:

        // xml保存
        virtual void save_xml( const bool backup );

        virtual Gtk::Menu* get_popupmenu( const std::string& url );
    };

    ///////////////////////////////////////

    class HistoryThreadView : public HistoryViewBase
    {
      public:

        HistoryThreadView( const std::string& url, const std::string& arg1 = std::string() , const std::string& arg2 = std::string() );
    };

    class HistoryCloseView : public HistoryViewBase
    {
      public:

        HistoryCloseView( const std::string& url, const std::string& arg1 = std::string() , const std::string& arg2 = std::string() );
    };

    class HistoryBoardView : public HistoryViewBase
    {
      public:

        HistoryBoardView( const std::string& url, const std::string& arg1 = std::string() , const std::string& arg2 = std::string() );
    };
}

#endif
