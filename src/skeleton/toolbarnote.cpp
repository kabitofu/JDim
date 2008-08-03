// ライセンス: GPL2

//#define _DEBUG
#include "jddebug.h"

#include "dragnote.h"
#include "toolbarnote.h"

#include <gtk/gtk.h>

using namespace SKELETON;


ToolBarNotebook::ToolBarNotebook( DragableNoteBook* parent )
    : Gtk::Notebook(),
      m_parent( parent )
{
    set_show_border( true );
    set_show_tabs( false );
    set_border_width( 0 );

    Glib::RefPtr< Gtk::RcStyle > rcst = get_modifier_style();
    rcst->set_ythickness( 1 );
    modify_style( rcst );
}


//
// 描画イベント
//
// 自前で枠を描画する
//
// gtknotebook.c( Revision 19593, Sat Feb 16 04:09:15 2008 UTC ) からのハック。環境やバージョンによっては問題が出るかもしれないので注意
//
bool ToolBarNotebook::on_expose_event( GdkEventExpose* event )
{
    const Glib::RefPtr<Gdk::Window> win = get_window();
    const Gdk::Rectangle rect( &(event->area) );
    const Alloc_NoteBook alloc = m_parent->get_alloc_notebook();

    const bool show_tab = m_parent->get_show_tabs();
    const int offset = ( show_tab ? alloc.height_tabbar - alloc.height_tab : 0 );

    const int x = get_allocation().get_x();
    const int y = get_allocation().get_y() - offset;
    const int w = get_allocation().get_width();
    const int h = offset + alloc.height_toolbar + alloc.height_view;
                                              
#ifdef _DEBUG
    std::cout << "ToolBarNotebook::on_expose_event"
              << " x_area = " << event->area.x
              << " y_area = " << event->area.y
              << " w_area = " << event->area.width
              << " h_area = " << event->area.height
              << " offset = " << offset 
              << " x = " << x
              << " y = " << y
              << " w = " << w
              << " h = " << h
              << std::endl;
#endif

    if( h > 0 ){

        if( show_tab ){
            get_style()->paint_box_gap( win,
                                        Gtk::STATE_NORMAL,
                                        Gtk::SHADOW_OUT,
                                        rect,
                                        *this,
                                        "notebook",
                                        x,
                                        y,
                                        w,
                                        h,
                                        Gtk::POS_TOP,
                                        alloc.x_tab,
                                        alloc.width_tab
                );
        }
        else{
            get_style()->paint_box( win,
                                    Gtk::STATE_NORMAL,
                                    Gtk::SHADOW_OUT,
                                    rect,
                                    *this,
                                    "notebook",
                                    x,
                                    y,
                                    w,
                                    h
                );
        }
    }

    // 枠は自前で書いたので gtk_notebook_expose では枠を描画させない
    GtkNotebook *notebook = gobj();
    notebook->show_border = false;
    bool ret = Notebook::on_expose_event( event );
    notebook->show_border = true;

    return ret;
}
