#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "ukmediaoutputwidget.h"
#include "ukmediaapplicationwidget.h"
#include "ukmediainputwidget.h"
#include "ukmediasoundeffectswidget.h"
extern "C" {
#include <gio/gio.h>
#include <libxml/tree.h>
#include <libmatemixer/matemixer.h>
}
#include <QMediaPlayer>
#define KEY_SOUNDS_SCHEMA "org.mate.sound"
#define EVENT_SOUNDS_KEY "event-sounds"
#define INPUT_SOUNDS_KEY "input-feedback-sounds"
#define SOUND_THEME_KEY "theme-name"

#define DEFAULT_ALERT_ID "__default"
#define CUSTOM_THEME_NAME "__custom"
#define NO_SOUNDS_THEME_NAME "__no_sounds"

typedef enum
{
    GVC_LEVEL_SCALE_LINEAR,
    GVC_LEVEL_SCALE_LOG
} LevelScale;
class UkmediaMainWidget : public QWidget
{
    Q_OBJECT

public:
    UkmediaMainWidget(QWidget *parent = nullptr);
    ~UkmediaMainWidget();

    static void list_device(UkmediaMainWidget *w,MateMixerContext *context);

    static void gvc_stream_status_icon_set_control (UkmediaMainWidget *w,MateMixerStreamControl *control);
    static void context_set_property(UkmediaMainWidget *object);//guint prop_id,const GValue *value,GParamSpec *pspec);
    static void on_context_state_notify (MateMixerContext *context,GParamSpec *pspec,UkmediaMainWidget	*w);

    static void on_context_stream_added (MateMixerContext *context,const gchar *name,UkmediaMainWidget  *w);
    static void on_context_stream_removed (MateMixerContext *context,const gchar *name,UkmediaMainWidget *w);
    static void remove_stream (UkmediaMainWidget *w, const gchar *name);

    static void add_stream (UkmediaMainWidget *w, MateMixerStream *stream,MateMixerContext *context);
    static void add_application_control (UkmediaMainWidget *w, MateMixerStreamControl *control);
    static void on_stream_control_added (MateMixerStream *stream,const gchar *name,UkmediaMainWidget  *w);
    static void on_stream_control_removed (MateMixerStream *stream,const gchar *name,UkmediaMainWidget *w);
    static void remove_application_control (UkmediaMainWidget *w,const gchar *name);
    static void add_app_to_appwidget(UkmediaMainWidget *w,int appnum,const gchar *app_name,QString app_icon_name,MateMixerStreamControl *control);
    static void on_context_stored_control_added (MateMixerContext *context,const gchar *name,UkmediaMainWidget *w);
    static void update_app_volume (MateMixerStreamControl *control, GParamSpec *pspec ,UkmediaMainWidget *w);

    static void on_context_device_added (MateMixerContext *context, const gchar *name, UkmediaMainWidget *w);
    static void add_device (UkmediaMainWidget *w, MateMixerDevice *device);

    static void on_context_device_removed (MateMixerContext *context,const gchar *name,UkmediaMainWidget *w);

    static void on_context_default_input_stream_notify (MateMixerContext *context,GParamSpec *pspec,UkmediaMainWidget *w);
    static void set_input_stream (UkmediaMainWidget *w, MateMixerStream *stream);
    static void on_stream_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,UkmediaMainWidget *w);

    static void on_context_default_output_stream_notify (MateMixerContext *context,GParamSpec *pspec,UkmediaMainWidget *w);

    static void on_context_stored_control_removed (MateMixerContext *context,const gchar *name,UkmediaMainWidget *w);
    static void set_context(UkmediaMainWidget *w,MateMixerContext *context);

    static void update_icon_input (UkmediaMainWidget *w);
    static void update_icon_output (UkmediaMainWidget *w);
    static void on_stream_control_volume_notify (MateMixerStreamControl *control,GParamSpec *pspec,UkmediaMainWidget *w);
    static void on_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,UkmediaMainWidget *w);
    //平衡
    static void gvc_balance_bar_set_property (UkmediaMainWidget *w,MateMixerStreamControl *control);
    static void gvc_balance_bar_set_control (UkmediaMainWidget *w, MateMixerStreamControl *control);
    static void gvc_balance_bar_set_balance_type (UkmediaMainWidget *w, MateMixerStreamControl *control);

    static void on_balance_value_changed (MateMixerStreamControl *control,GParamSpec *pspec,UkmediaMainWidget *w);
    static void update_output_settings (UkmediaMainWidget *w,MateMixerStreamControl *control);

    static void on_key_changed (GSettings *settings,gchar *key,UkmediaMainWidget *w);
    static void update_theme (UkmediaMainWidget *w);

    static void setup_theme_selector (UkmediaMainWidget *w);
    static void sound_theme_in_dir (UkmediaMainWidget *w,GHashTable *hash,const char *dir);
    static char *load_index_theme_name (const char *index,char **parent);

    static void set_combox_for_theme_name (UkmediaMainWidget *w,const char *name);
    static void update_alerts_from_theme_name (UkmediaMainWidget *w,const gchar *name);
    static void update_alert (UkmediaMainWidget *w,const char *alert_id);
    static int get_file_type (const char *sound_name,char **linked_name);
    static char *custom_theme_dir_path (const char *child);

    static void populate_model_from_dir (UkmediaMainWidget *w,const char *dirname);
    static void populate_model_from_file (UkmediaMainWidget *w,const char *filename);
    static void populate_model_from_node (UkmediaMainWidget *w,xmlNodePtr node);
    static xmlChar *xml_get_and_trim_names (xmlNodePtr node);

    static void play_alret_sound_from_path (QString path);
    static void set_output_stream (UkmediaMainWidget *w, MateMixerStream *stream);
    static void update_output_stream_list(UkmediaMainWidget *w,MateMixerStream *stream);

    static void bar_set_stream (UkmediaMainWidget *w,MateMixerStream *stream);
    static void bar_set_stream_control (UkmediaMainWidget *w,MateMixerStreamControl *control);

    static void update_input_settings (UkmediaMainWidget *w,MateMixerStreamControl *control);
    static void on_stream_control_monitor_value (MateMixerStream *stream,gdouble value,UkmediaMainWidget *w);
    void input_level_set_property (UkmediaMainWidget *w);
    void input_level_set_scale (UkmediaMainWidget *w, LevelScale scale);
    static void update_peak_value (UkmediaMainWidget *w);
    static void update_rms_value (UkmediaMainWidget *w);

    static gdouble fraction_from_adjustment (UkmediaMainWidget   *w);

    static void on_input_stream_control_added (MateMixerStream *stream,const gchar *name,UkmediaMainWidget *w);
    static void on_input_stream_control_removed (MateMixerStream *stream,const gchar *name,UkmediaMainWidget *w);
    static gboolean update_default_input_stream (UkmediaMainWidget *w);

Q_SIGNALS:
    void app_volume_changed(bool is_mute,int volume,const gchar *app_name);

private Q_SLOTS:
    void app_slider_changed_slot(int volume);
    void app_volume_changed_slot(bool is_mute,int volume,const gchar *app_name);
    void output_volume_slider_changed_slot(int volume);
    void input_volume_slider_changed_slot(int volume);
    void combox_index_changed_slot(int index);
    void theme_combox_index_changed_slot(int index);
    void output_device_combox_index_changed_slot(int index);
    void input_device_combox_index_changed_slot(int index);
    void input_level_value_changed_slot();
private:
    UkmediaApplicationWidget *appWidget;
    UkmediaInputWidget *inputWidget;
    UkmediaOutputWidget *outputWidget;
    UkmediaSoundEffectsWidget *soundWidget;

    MateMixerContext *context;
    MateMixerStream *stream;
    MateMixerStream *input;
    MateMixerStream *output;
    MateMixerStreamControl *control;

    QStringList *soundlist;
    QStringList *theme_display_name_list;
    QStringList *theme_name_list;
    QStringList *input_device_name_list;
    QStringList *output_device_name_list;
    QStringList *input_device_display_name_list;
    QStringList *output_device_display_name_list;
    QStringList *output_stream_list;
    QStringList *input_stream_list;
    QStringList *app_volume_list;
    QStringList *stream_control_list;

    QMediaPlayer *player;
    GSettings *sound_settings;
//    QLabel *app_display_label;

    LevelScale scale;
    gdouble peak_fraction;
    gdouble rms_fraction;
    gdouble max_peak;

    guint max_peak_id;
    QWidget *app_widget;
//    QGridLayout *gridlayout;
};

#endif // WIDGET_H
