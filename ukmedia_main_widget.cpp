#include "ukmedia_main_widget.h"
#include <XdgIcon>
#include <XdgDesktopFile>
#include <QDebug>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QStringList>
#include <QSpacerItem>
#include <QListView>
#include <QScrollBar>

#define MATE_DESKTOP_USE_UNSTABLE_API
#define VERSION "1.12.1"
#define GVC_DIALOG_DBUS_NAME "org.mate.VolumeControl"
#define KEY_SOUNDS_SCHEMA   "org.mate.sound"
#define GVC_SOUND_SOUND    (xmlChar *) "sound"
#define GVC_SOUND_NAME     (xmlChar *) "name"
#define GVC_SOUND_FILENAME (xmlChar *) "filename"
#define SOUND_SET_DIR "/usr/share/ukui-media/sounds"
guint appnum = 0;
typedef enum {
    BALANCE_TYPE_RL,
    BALANCE_TYPE_FR,
    BALANCE_TYPE_LFE,
} GvcBalanceType;

enum {
    SOUND_TYPE_UNSET,
    SOUND_TYPE_OFF,
    SOUND_TYPE_DEFAULT_FROM_THEME,
    SOUND_TYPE_BUILTIN,
    SOUND_TYPE_CUSTOM
};

UkmediaMainWidget::UkmediaMainWidget(QWidget *parent)
    : QWidget(parent)
{
    m_pOutputWidget = new UkmediaOutputWidget(this);
    m_pInputWidget = new UkmediaInputWidget(this);
    m_pAppWidget = new UkmediaApplicationWidget(this);
    m_pSoundWidget = new UkmediaSoundEffectsWidget(this);

    QVBoxLayout *m_pvLayout = new QVBoxLayout(this);
    m_pvLayout->addWidget(m_pOutputWidget);
    m_pvLayout->addWidget(m_pInputWidget);
    m_pvLayout->addWidget(m_pSoundWidget);
    m_pvLayout->addWidget(m_pAppWidget);
    m_pvLayout->setSpacing(48);
    this->setLayout(m_pvLayout);
    this->setMinimumWidth(582);
    this->setMaximumWidth(910);
    this->setStyleSheet("QWidget{background: white;}");

    if (mate_mixer_init() == FALSE) {
        qDebug() << "libmatemixer initialization failed, exiting";
    }

    m_pSoundList = new QStringList;
    m_pThemeNameList = new QStringList;
    m_pThemeDisplayNameList = new QStringList;
    m_pInputDeviceNameList = new QStringList;
    m_pOutputDeviceNameList = new QStringList;
    m_pOutputStreamList = new QStringList;
    m_pInputStreamList = new QStringList;
    m_pAppVolumeList = new QStringList;
    m_pStreamControlList = new QStringList;

    //创建context
    m_pContext = mate_mixer_context_new();

    mate_mixer_context_set_app_name (m_pContext,_("Volume Control"));//设置app名
    mate_mixer_context_set_app_icon(m_pContext,"multimedia-volume-control");

    //打开context
    if G_UNLIKELY (mate_mixer_context_open(m_pContext) == FALSE) {
        qDebug() << "open context fail";
        g_warning ("Failed to connect to a sound system**********************");
    }

    g_param_spec_object ("context",
                        "Context",
                        "MateMixer context",
                        MATE_MIXER_TYPE_CONTEXT,
                        (GParamFlags)(G_PARAM_READWRITE |
                        G_PARAM_CONSTRUCT_ONLY |
                        G_PARAM_STATIC_STRINGS));

    if (mate_mixer_context_get_state (m_pContext) == MATE_MIXER_STATE_CONNECTING) {

    }

    contextSetProperty(this);
    g_signal_connect (G_OBJECT (m_pContext),
                     "notify::state",
                     G_CALLBACK (onContextStateNotify),
                     this);

    //设置滑动条的最大值为100
    m_pInputWidget->m_pIpVolumeSlider->setMaximum(100);
    m_pOutputWidget->m_pOpVolumeSlider->setMaximum(100);
    m_pOutputWidget->m_pOpBalanceSlider->setMaximum(100);
    m_pOutputWidget->m_pOpBalanceSlider->setMinimum(-100);
    m_pOutputWidget->m_pOpBalanceSlider->setSingleStep(100);
    m_pInputWidget->m_pInputLevelSlider->setMaximum(100);
    m_pInputWidget->m_pInputLevelSlider->setEnabled(false);
    //设置声音主题
    //获取声音gsettings值
    m_pSoundSettings = g_settings_new (KEY_SOUNDS_SCHEMA);

    g_signal_connect (G_OBJECT (m_pSoundSettings),
                             "changed",
                             G_CALLBACK (onKeyChanged),
                             this);
    setupThemeSelector(this);
    updateTheme(this);

    //报警声音,从指定路径获取报警声音文件
    populateModelFromDir(this,SOUND_SET_DIR);

    //点击报警音量时播放报警声音
    connect(m_pSoundWidget->m_pShutdownCombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(comboxIndexChangedSlot(int)));
    connect(m_pSoundWidget->m_pLagoutCombobox ,SIGNAL(currentIndexChanged(int)),this,SLOT(comboxIndexChangedSlot(int)));
    connect(m_pSoundWidget->m_pSoundThemeCombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(themeComboxIndexChangedSlot(int)));
    connect(m_pInputWidget->m_pInputLevelSlider,SIGNAL(valueChanged(int)),this,SLOT(inputLevelValueChangedSlot()));
    //输入等级
    ukuiInputLevelSetProperty(this);

    m_pOutputWidget->m_pOutputDeviceCombobox->setView(new QListView());
    m_pInputWidget->m_pInputDeviceCombobox->setView(new QListView());
    m_pSoundWidget->m_pSoundThemeCombobox->setView(new QListView());
    m_pSoundWidget->m_pShutdownCombobox->setView(new QListView());
    m_pSoundWidget->m_pLagoutCombobox->setView(new QListView());
}

/*
 * context状态通知
*/
void UkmediaMainWidget::onContextStateNotify (MateMixerContext *m_pContext,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("on context state notify");
    MateMixerState state = mate_mixer_context_get_state (m_pContext);
    listDevice(m_pWidget,m_pContext);
    if (state == MATE_MIXER_STATE_READY) {
        updateDefaultInputStream (m_pWidget);
        updateIconOutput(m_pWidget);
        updateIconInput(m_pWidget);
    }
    else if (state == MATE_MIXER_STATE_FAILED) {
        g_debug(" mate mixer state failed");
    }
    //点击输出设备
    connect(m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox,SIGNAL(currentIndexChanged(int)),m_pWidget,SLOT(outputDeviceComboxIndexChangedSlot(int)));
    //点击输入设备
    connect(m_pWidget->m_pInputWidget->m_pInputDeviceCombobox,SIGNAL(currentIndexChanged(int)),m_pWidget,SLOT(inputDeviceComboxIndexChangedSlot(int)));
}

/*
    context 存储control增加
*/
void UkmediaMainWidget::onContextStoredControlAdded(MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    g_debug("on conext stored control add");
    MateMixerStreamControl *m_pControl;
    MateMixerStreamControlMediaRole mediaRole;
    m_pControl = MATE_MIXER_STREAM_CONTROL (mate_mixer_context_get_stored_control (m_pContext, m_pName));
    if (G_UNLIKELY (m_pControl == nullptr))
        return;

    mediaRole = mate_mixer_stream_control_get_media_role (m_pControl);
    if (mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT)
        ukuiBarSetStreamControl (m_pWidget, m_pControl);
}

/*
    当其他设备插入时添加这个stream
*/
void UkmediaMainWidget::onContextStreamAdded (MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    g_debug("on context stream added");
    MateMixerStream *m_pStream;
    m_pStream = mate_mixer_context_get_stream (m_pContext, m_pName);
    if (G_UNLIKELY (m_pStream == nullptr))
        return;
    addStream (m_pWidget, m_pStream,m_pContext);
}

/*
列出设备
*/
void UkmediaMainWidget::listDevice(UkmediaMainWidget *m_pWidget,MateMixerContext *m_pContext)
{
    g_debug("list device");
    const GList *m_pList;
    m_pList = mate_mixer_context_list_streams (m_pContext);

    while (m_pList != nullptr) {
        addStream (m_pWidget, MATE_MIXER_STREAM (m_pList->data),m_pContext);
        MateMixerStream *m_pStream = MATE_MIXER_STREAM(m_pList->data);
        const gchar *m_pStreamName = mate_mixer_stream_get_name(m_pStream);

        MateMixerDirection direction = mate_mixer_stream_get_direction(m_pStream);
        if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
            m_pWidget->m_pOutputStreamList->append(m_pStreamName);
        }
        else if (direction == MATE_MIXER_DIRECTION_INPUT) {
            m_pWidget->m_pInputStreamList->append(m_pStreamName);
        }
        m_pList = m_pList->next;
    }
}

void UkmediaMainWidget::addStream (UkmediaMainWidget *m_pWidget, MateMixerStream *m_pStream,MateMixerContext *m_pContext)
{
    g_debug("add stream");
    const GList *m_pControls;
    MateMixerDirection direction;
    direction = mate_mixer_stream_get_direction (m_pStream);
    const gchar *m_pName;
    const gchar *m_pLabel;
    MateMixerStreamControl *m_pControl;
    if (direction == MATE_MIXER_DIRECTION_INPUT) {
        MateMixerStream *m_pInput;
        m_pInput = mate_mixer_context_get_default_input_stream (m_pContext);
        if (m_pStream == m_pInput) {
            ukuiBarSetStream (m_pWidget, m_pStream);
            m_pControl = mate_mixer_stream_get_default_control(m_pStream);
            updateInputSettings (m_pWidget,m_pControl);
        }
        m_pName  = mate_mixer_stream_get_name (m_pStream);
        m_pLabel = mate_mixer_stream_get_label (m_pStream);
        m_pWidget->m_pInputDeviceNameList->append(m_pName);
        m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->addItem(m_pLabel);
    }
    else if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        MateMixerStream        *m_pOutput;
        MateMixerStreamControl *m_pControl;
        m_pOutput = mate_mixer_context_get_default_output_stream (m_pContext);
        m_pControl = mate_mixer_stream_get_default_control (m_pStream);
        if (m_pStream == m_pOutput) {
            updateOutputSettings(m_pWidget,m_pControl);
            ukuiBarSetStream (m_pWidget, m_pStream);
        }
        m_pName  = mate_mixer_stream_get_name (m_pStream);
        m_pLabel = mate_mixer_stream_get_label (m_pStream);
        m_pWidget->m_pOutputDeviceNameList->append(m_pName);
        m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->addItem(m_pLabel);
    }
    m_pControls = mate_mixer_stream_list_controls (m_pStream);
    while (m_pControls != nullptr) {
        MateMixerStreamControl    *m_pControl = MATE_MIXER_STREAM_CONTROL (m_pControls->data);
        MateMixerStreamControlRole role;
        role = mate_mixer_stream_control_get_role (m_pControl);
        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            MateMixerAppInfo *m_pAppInfo = mate_mixer_stream_control_get_app_info(m_pControl);
            const gchar *m_pAppName = mate_mixer_app_info_get_name(m_pAppInfo);
            if (strcmp(m_pAppName,"ukui-session") != 0) {
                m_pWidget->m_pStreamControlList->append(m_pName);
                if G_UNLIKELY (m_pControl == nullptr)
                    return;
                addApplicationControl (m_pWidget, m_pControl);
            }
        }
        m_pControls = m_pControls->next;
    }

    // XXX find a way to disconnect when removed
    g_signal_connect (G_OBJECT (m_pStream),
                      "control-added",
                      G_CALLBACK (addApplicationControl),
                      m_pWidget);
    g_signal_connect (G_OBJECT (m_pStream),
                      "control-removed",
                      G_CALLBACK (onStreamControlRemoved),
                      m_pWidget);
}

/*
    添加应用音量控制
*/
void UkmediaMainWidget::addApplicationControl (UkmediaMainWidget *m_pWidget, MateMixerStreamControl *m_pControl)
{
    g_debug("add application control");
    MateMixerStream *m_pStream;
    MateMixerStreamControlMediaRole mediaRole;
    MateMixerAppInfo *m_pInfo;
    MateMixerDirection direction = MATE_MIXER_DIRECTION_UNKNOWN;
    const gchar *m_pAppId;
    const gchar *m_pAppName;
    const gchar *m_pAppIcon;
    appnum++;
    mediaRole = mate_mixer_stream_control_get_media_role (m_pControl);

    /* Add stream to the applications page, but make sure the stream qualifies
     * for the inclusion */
    m_pInfo = mate_mixer_stream_control_get_app_info (m_pControl);
    if (m_pInfo == nullptr)
        return;

    /* Skip streams with roles we don't care about */
    if (mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT ||
        mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_TEST ||
        mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_ABSTRACT ||
        mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_FILTER)
            return;

    m_pAppId = mate_mixer_app_info_get_id (m_pInfo);

    /* These applications may have associated streams because they do peak
     * level monitoring, skip these too */
    if (!g_strcmp0 (m_pAppId, "org.mate.VolumeControl") ||
        !g_strcmp0 (m_pAppId, "org.gnome.VolumeControl") ||
        !g_strcmp0 (m_pAppId, "org.PulseAudio.pavucontrol"))
        return;

    QString app_icon_name = mate_mixer_app_info_get_icon(m_pInfo);

    m_pAppName = mate_mixer_app_info_get_name (m_pInfo);
    addAppToAppwidget(m_pWidget,appnum,m_pAppName,app_icon_name,m_pControl);

    if (m_pAppName == nullptr)
        m_pAppName = mate_mixer_stream_control_get_label (m_pControl);
    if (m_pAppName == nullptr)
        m_pAppName = mate_mixer_stream_control_get_name (m_pControl);
    if (G_UNLIKELY (m_pAppName == nullptr))
        return;

    /* By default channel bars use speaker icons, use microphone icons
     * instead for recording applications */
    m_pStream = mate_mixer_stream_control_get_stream (m_pControl);
    if (m_pStream != nullptr)
        direction = mate_mixer_stream_get_direction (m_pStream);

    if (direction == MATE_MIXER_DIRECTION_INPUT) {

    }
    m_pAppIcon = mate_mixer_app_info_get_icon (m_pInfo);
    if (m_pAppIcon == nullptr) {
        if (direction == MATE_MIXER_DIRECTION_INPUT)
            m_pAppIcon = "audio-input-microphone";
        else
            m_pAppIcon = "applications-multimedia";
    }
    ukuiBarSetStreamControl (m_pWidget, m_pControl);
}

void UkmediaMainWidget::onStreamControlAdded (MateMixerStream *m_pStream,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    g_debug("on stream control added");
    MateMixerStreamControl    *m_pControl;
    MateMixerStreamControlRole role;

    m_pControl = mate_mixer_stream_get_control (m_pStream, m_pName);
    if G_UNLIKELY (m_pControl == nullptr)
        return;

    MateMixerAppInfo *m_pAppInfo = mate_mixer_stream_control_get_app_info(m_pControl);
    const gchar *m_pAppName = mate_mixer_app_info_get_name(m_pAppInfo);
    if (strcmp(m_pAppName,"ukui-session") != 0) {
        m_pWidget->m_pStreamControlList->append(m_pName);
        if G_UNLIKELY (m_pControl == nullptr)
            return;

        role = mate_mixer_stream_control_get_role (m_pControl);
        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            addApplicationControl(m_pWidget, m_pControl);
        }
    }



}

/*
    移除control
*/
void UkmediaMainWidget::onStreamControlRemoved (MateMixerStream *m_pStream,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pStream);
    g_debug("on stream control removed");
    removeApplicationControl(m_pWidget, m_pName);
}

void UkmediaMainWidget::removeApplicationControl(UkmediaMainWidget *m_pWidget,const gchar *m_pName)
{
    g_debug ("Removing application stream %s", m_pName);
    int i = m_pWidget->m_pStreamControlList->indexOf(m_pName);

    m_pWidget->m_pStreamControlList->removeAt(i);
    //当播放音乐的应用程序退出后删除该项
    QLayoutItem *item ;
    if ((item = m_pWidget->m_pAppWidget->m_pGridlayout->takeAt(i)) != 0) {
        item->widget()->setParent(nullptr);
        delete item;
    }
    m_pWidget->m_pAppWidget->m_pGridlayout->update();
    if (appnum <= 0) {
        g_warn_if_reached ();
        appnum = 1;
    }
    appnum--;
    m_pWidget->m_pAppWidget->m_pGridlayout->setContentsMargins(0,0,0,m_pWidget->m_pAppWidget->m_pAppWid->height()-appnum*52);
    if (appnum <= 0)
        m_pWidget->m_pAppWidget->m_pNoAppLabel->show();
    else
        m_pWidget->m_pAppWidget->m_pNoAppLabel->hide();

}

void UkmediaMainWidget::addAppToAppwidget(UkmediaMainWidget *m_pWidget,int appnum, const gchar *m_pAppName,QString appIconName,MateMixerStreamControl *m_pControl)
{
    g_debug("add app to widget");
    //获取应用静音状态及音量
    int volume = 0;
    gboolean isMute = false;
    gdouble normal = 0.0;
    QString percent;
    isMute = mate_mixer_stream_control_get_mute(m_pControl);
    volume = mate_mixer_stream_control_get_volume(m_pControl);
    normal = mate_mixer_stream_control_get_normal_volume(m_pControl);
    int displayVolume = 100 * volume / normal;

    //设置应用的图标
    QString iconName = "/usr/share/applications/";
    iconName.append(appIconName);
    iconName.append(".desktop");
    XdgDesktopFile xdg;
    xdg.load(iconName);
    GError **error = nullptr;
    GKeyFileFlags flags = G_KEY_FILE_NONE;
    GKeyFile *keyflie = g_key_file_new();

    g_key_file_load_from_file(keyflie,iconName.toLocal8Bit(),flags,error);
    char *m_pIconStr = g_key_file_get_locale_string(keyflie,"Desktop Entry","Icon",nullptr,nullptr);
    QIcon icon = QIcon::fromTheme(QString::fromLocal8Bit(m_pIconStr));
    m_pWidget->m_pAppVolumeList->append(appIconName);

    //widget显示应用音量
    m_pWidget->m_pApplicationWidget = new QWidget(m_pWidget->m_pAppWidget->m_pAppWid);
    m_pWidget->m_pApplicationWidget->setMinimumSize(550,50);
    m_pWidget->m_pApplicationWidget->setMaximumSize(960,50);
    QHBoxLayout *m_pHlayout = new QHBoxLayout(m_pWidget->m_pApplicationWidget);
    m_pWidget->m_pAppWidget->m_pAppLabel = new QLabel(m_pWidget->m_pApplicationWidget);
    m_pWidget->m_pAppWidget->m_pAppIconBtn = new QPushButton(m_pWidget->m_pApplicationWidget);
    m_pWidget->m_pAppWidget->m_pAppIconLabel = new QLabel(m_pWidget->m_pApplicationWidget);
    m_pWidget->m_pAppWidget->m_pAppVolumeLabel = new QLabel(m_pWidget->m_pApplicationWidget);
    m_pWidget->m_pAppWidget->m_pAppSlider = new AudioSlider(m_pWidget->m_pApplicationWidget);
    m_pWidget->m_pAppWidget->m_pAppSlider->setOrientation(Qt::Horizontal);

    //设置每项的固定大小
    m_pWidget->m_pAppWidget->m_pAppLabel->setFixedSize(160,14);
    m_pWidget->m_pAppWidget->m_pAppIconBtn->setFixedSize(32,32);
    m_pWidget->m_pAppWidget->m_pAppIconLabel->setFixedSize(24,24);
    m_pWidget->m_pAppWidget->m_pAppVolumeLabel->setFixedSize(36,14);

    QSpacerItem *item1 = new QSpacerItem(16,20);
    QSpacerItem *item2 = new QSpacerItem(8,20);

    m_pWidget->m_pAppWidget->m_pAppVolumeLabel->setFixedHeight(16);
    m_pHlayout->addItem(item1);
    m_pHlayout->addWidget(m_pWidget->m_pAppWidget->m_pAppIconBtn);
    m_pHlayout->addItem(item2);
    m_pHlayout->addWidget(m_pWidget->m_pAppWidget->m_pAppLabel);
    m_pHlayout->addItem(item1);
    m_pHlayout->addWidget(m_pWidget->m_pAppWidget->m_pAppIconLabel);
    m_pHlayout->addItem(item1);
    m_pHlayout->addWidget(m_pWidget->m_pAppWidget->m_pAppSlider);
    m_pHlayout->addItem(item1);
    m_pHlayout->addWidget(m_pWidget->m_pAppWidget->m_pAppVolumeLabel);
    m_pHlayout->addItem(item1);
    m_pWidget->m_pApplicationWidget->setLayout(m_pHlayout);
    m_pWidget->m_pApplicationWidget->layout()->setContentsMargins(0,0,0,0);
    m_pHlayout->setSpacing(0);

//    hlayout->addWidget(app_widget);
    //添加widget到gridlayout中
    m_pWidget->m_pAppWidget->m_pGridlayout->addWidget(m_pWidget->m_pApplicationWidget);
    m_pWidget->m_pAppWidget->m_pGridlayout->setVerticalSpacing(1);
    m_pWidget->m_pAppWidget->m_pGridlayout->setContentsMargins(0,0,0,m_pWidget->m_pAppWidget->height()-appnum*52);

    QSize icon_size(32,32);
    m_pWidget->m_pAppWidget->m_pAppIconBtn->setIconSize(icon_size);
    m_pWidget->m_pAppWidget->m_pAppIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");
    m_pWidget->m_pAppWidget->m_pAppIconBtn->setIcon(icon);
//    m_pWidget->appIconBtn->setFlat(true);
//    m_pWidget->appIconBtn->setEnabled(true);
    m_pWidget->m_pAppWidget->m_pAppIconBtn->setFocusPolicy(Qt::NoFocus);

    m_pWidget->m_pAppWidget->m_pAppSlider->setMaximum(100);
    m_pWidget->m_pAppWidget->m_pAppSlider->setMinimumSize(178,20);
    m_pWidget->m_pAppWidget->m_pAppSlider->setMaximumSize(800,20);

    QString appSliderStr = m_pAppName;
    QString appLabelStr = m_pAppName;
    QString appVolumeLabelStr = m_pAppName;
    QString appIconLabelStr = m_pAppName;

    appSliderStr.append("Slider");
    appLabelStr.append("Label");
    appVolumeLabelStr.append("VolumeLabel");
    appIconLabelStr.append("VolumeIconLabel");
    m_pWidget->m_pAppWidget->m_pAppSlider->setObjectName(appSliderStr);
    m_pWidget->m_pAppWidget->m_pAppLabel->setObjectName(appLabelStr);
    m_pWidget->m_pAppWidget->m_pAppVolumeLabel->setObjectName(appVolumeLabelStr);
    m_pWidget->m_pAppWidget->m_pAppIconLabel->setObjectName(appIconLabelStr);
    percent = QString::number(displayVolume);
    percent.append("%");
    //设置label 和滑动条的值
    m_pWidget->m_pAppWidget->m_pAppLabel->setText(m_pAppName);
    m_pWidget->m_pAppWidget->m_pAppSlider->setValue(displayVolume);
    m_pWidget->m_pAppWidget->m_pAppVolumeLabel->setText(percent);
    //设置声音标签图标
    QPixmap pix;
    if (isMute) {
        m_pWidget->m_pAppWidget->m_pAppSlider->setValue(displayVolume);
        mate_mixer_stream_control_set_mute(m_pControl,isMute);
    }
    if (displayVolume <= 0) {
        pix = QPixmap("/usr/share/ukui-media/img/audio-volume-muted.svg");
        m_pWidget->m_pAppWidget->m_pAppIconLabel->setPixmap(pix);
    }
    else if (displayVolume > 0 && displayVolume <= 33) {
        pix = QPixmap("/usr/share/ukui-media/img/audio-volume-low.svg");
        m_pWidget->m_pAppWidget->m_pAppIconLabel->setPixmap(pix);
    }
    else if (displayVolume >33 && displayVolume <= 66) {
        pix = QPixmap("/usr/share/ukui-media/img/audio-volume-medium.svg");
        m_pWidget->m_pAppWidget->m_pAppIconLabel->setPixmap(pix);
    }
    else {
        pix = QPixmap("/usr/share/ukui-media/img/audio-volume-high.svg");
        m_pWidget->m_pAppWidget->m_pAppIconLabel->setPixmap(pix);
    }

    /*滑动条控制应用音量*/
    connect(m_pWidget->m_pAppWidget->m_pAppSlider,&QSlider::valueChanged,[=](int value){
        QSlider *m_pSlider= m_pWidget->findChild<QSlider*>(appSliderStr);
        m_pSlider->setValue(value);
        QLabel *m_pLabel = m_pWidget->findChild<QLabel*>(appVolumeLabelStr);
        QString percent;
        percent = QString::number(value);
        percent.append("%");
        m_pLabel->setText(percent);
        QLabel *appIcon = m_pWidget->findChild<QLabel*>(appIconLabelStr);

        int volume = int(value*65536/100);
        mate_mixer_stream_control_set_volume(m_pControl,(int)volume);
        //设置声音标签图标
        QPixmap pix;
        if (value <= 0) {
            pix = QPixmap("/usr/share/ukui-media/img/audio-volume-muted.svg");
            appIcon->setPixmap(pix);
        }
        else if (value > 0 && value <= 33) {
            pix = QPixmap("/usr/share/ukui-media/img/audio-volume-low.svg");
            appIcon->setPixmap(pix);
        }
        else if (value >33 && value <= 66) {
            pix = QPixmap("/usr/share/ukui-media/img/audio-volume-medium.svg");
            appIcon->setPixmap(pix);
        }
        else {
            pix = QPixmap("/usr/share/ukui-media/img/audio-volume-high.svg");
            appIcon->setPixmap(pix);
        }
    });
    /*应用音量同步*/
    g_signal_connect (G_OBJECT (m_pControl),
                     "notify::volume",
                     G_CALLBACK (updateAppVolume),
                     m_pWidget);

//    g_signal_connect (G_OBJECT (control),
//                     "notify::muted",
//                     G_CALLBACK (app_volume_mute),
//                     w);

    connect(m_pWidget,&UkmediaMainWidget::appVolumeChangedSignal,[=](bool isMute,int volume,const gchar *app_name){
        Q_UNUSED(isMute);
        QString slider_str = app_name;
        slider_str.append("Slider");
        QSlider *s = m_pWidget->findChild<QSlider*>(slider_str);
        s->setValue(volume);
    });

    m_pWidget->m_pApplicationWidget->setStyleSheet("QWidget{"
                              "background: rgba(244,244,244,1);"
                              "border-radius: 4px;}");

    if (appnum <= 0)
        m_pWidget->m_pAppWidget->m_pNoAppLabel->show();
    else
        m_pWidget->m_pAppWidget->m_pNoAppLabel->hide();

    m_pWidget->m_pAppWidget->m_pAppSlider->setStyleSheet("QSlider::groove:horizontal {"
                                           "border: 0px solid #bbb; }"
                                           "QSlider::sub-page:horizontal {"
                                           "background: #2e8ad2;border-radius: 2px;"
                                           "margin-top:8px;margin-bottom:9px;}"
                                           "QSlider::add-page:horizontal {"
                                           "background:  rgba(52,70,80,90%);"
                                           "border: 0px solid #777;"
                                           "border-radius: 2px;"
                                           "margin-top:8px;"
                                           "margin-bottom:9px;}"
                                           "QSlider::handle:horizontal {"
                                           "width: 20px;"
                                           "height: 20px;"
                                           "background: rgb(61,107,229);"
                                           "border-radius:10px;}");
}

/*
    同步应用音量
*/
void UkmediaMainWidget::updateAppVolume(MateMixerStreamControl *m_pControl, GParamSpec *pspec, UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("update app volume");
    guint value = mate_mixer_stream_control_get_volume(m_pControl);
    guint volume ;
    volume = guint(value*100/65536.0+0.5);
    bool isMute = mate_mixer_stream_control_get_mute(m_pControl);
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(m_pControl);
    MateMixerAppInfo *info = mate_mixer_stream_control_get_app_info(m_pControl);
    const gchar *m_pAppName = mate_mixer_app_info_get_name(info);
    Q_EMIT m_pWidget->appVolumeChangedSignal(isMute,volume,m_pAppName);

    //静音可读并且处于静音
    if ((control_flags & MATE_MIXER_STREAM_CONTROL_MUTE_WRITABLE) ) {
    }
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE) {
        //设置滑动条的值
//        Q_EMIT->emitVolume(volume);
    }
}

/*
    连接context，处理不同信号
*/
void UkmediaMainWidget::setContext(UkmediaMainWidget *m_pWidget,MateMixerContext *m_pContext)
{
    g_debug("set context");
    g_signal_connect (G_OBJECT (m_pContext),
                      "stream-added",
                      G_CALLBACK (onContextStreamAdded),
                      m_pWidget);

    g_signal_connect (G_OBJECT (m_pContext),
                    "stream-removed",
                    G_CALLBACK (onContextStreamRemoved),
                    m_pWidget);

    g_signal_connect (G_OBJECT (m_pContext),
                    "device-added",
                    G_CALLBACK (onContextDeviceAdded),
                    m_pWidget);
    g_signal_connect (G_OBJECT (m_pContext),
                    "device-removed",
                    G_CALLBACK (onContextDeviceRemoved),
                    m_pWidget);

    g_signal_connect (G_OBJECT (m_pContext),
                    "notify::default-input-stream",
                    G_CALLBACK (onContextDefaultInputStreamNotify),
                    m_pWidget);
    g_signal_connect (G_OBJECT (m_pContext),
                    "notify::default-output-stream",
                    G_CALLBACK (onContextDefaultOutputStreamNotify),
                    m_pWidget);

    g_signal_connect (G_OBJECT (m_pContext),
                    "stored-control-added",
                    G_CALLBACK (onContextStoredControlAdded),
                    m_pWidget);
    g_signal_connect (G_OBJECT (m_pContext),
                    "stored-control-removed",
                    G_CALLBACK (onContextStoredControlRemoved),
                    m_pWidget);

}

/*
    remove stream
*/
void UkmediaMainWidget::onContextStreamRemoved (MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pContext);
    Q_UNUSED(m_pName);
    g_debug("on context stream removed");
    removeStream (m_pWidget, m_pName);
}

/*
    移除stream
*/
void UkmediaMainWidget::removeStream (UkmediaMainWidget *m_pWidget, const gchar *m_pName)
{
    g_debug("remove stream");
    MateMixerStream *m_pStream = mate_mixer_context_get_stream(m_pWidget->m_pContext,m_pName);
    MateMixerDirection direction = mate_mixer_stream_get_direction(m_pStream);
    if (direction == MATE_MIXER_DIRECTION_INPUT) {
        m_pWidget->m_pInputStreamList->removeOne(m_pName);
    }
    else if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        m_pWidget->m_pOutputStreamList->removeOne(m_pName);
    }

    if (m_pWidget->m_pAppVolumeList != nullptr) {
        ukuiBarSetStream(m_pWidget,nullptr);
    }
}

/*
    context 添加设备并设置到单选框
*/
void UkmediaMainWidget::onContextDeviceAdded(MateMixerContext *m_pContext, const gchar *m_pName, UkmediaMainWidget *m_pWidget)
{
    g_debug("on context device added");
    MateMixerDevice *m_pDevice;
    m_pDevice = mate_mixer_context_get_device (m_pContext, m_pName);

    if (G_UNLIKELY (m_pDevice == nullptr))
        return;
    addDevice (m_pWidget, m_pDevice);
}

/*
    添加设备
*/
void UkmediaMainWidget::addDevice (UkmediaMainWidget *m_pWidget, MateMixerDevice *m_pDevice)
{
    g_debug("add device");
    const gchar *m_pName;
    const gchar *m_pLabel;
    m_pName  = mate_mixer_device_get_name (m_pDevice);
    m_pLabel = mate_mixer_device_get_label (m_pDevice);
    m_pWidget->m_pInputDeviceNameList->append(m_pName);
    m_pWidget->m_pOutputDeviceNameList->append(m_pName);
    //添加设备到组合框
    m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->addItem(m_pLabel);
    m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->addItem(m_pLabel);
}

/*
    移除设备
*/
void UkmediaMainWidget::onContextDeviceRemoved (MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    g_debug("on context device removed");
    int  count = 0;
    MateMixerDevice *m_pDevice = mate_mixer_context_get_device(m_pContext,m_pName);
    mate_mixer_device_get_label(m_pDevice);
    do {
        if (m_pName == m_pWidget->m_pInputDeviceNameList->at(count)) {
            qDebug() << "device remove";
            m_pWidget->m_pOutputDeviceNameList->removeAt(count);
            m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->removeItem(count);
            break;
        }
        else if (m_pName == m_pWidget->m_pInputDeviceNameList->at(count)) {
            m_pWidget->m_pInputDeviceNameList->removeAt(count);
            m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->removeItem(count);
            break;
        }
        count++;
        if (count > m_pWidget->m_pInputDeviceNameList->size() || count > m_pWidget->m_pOutputDeviceNameList->size()  ) {
            qDebug() << "device error";
            break;
        }
    }while(1);

    m_pWidget->m_pInputWidget->m_pInputDeviceCombobox->removeItem(1);
    m_pWidget->m_pOutputWidget->m_pOutputDeviceCombobox->removeItem(1);
    if (m_pDevice == nullptr)
        qDebug() << "device is null";
}

/*
    默认输入流通知
*/
void UkmediaMainWidget::onContextDefaultInputStreamNotify (MateMixerContext *m_pContext,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug ("on context default input stream notify");
    MateMixerStream *m_pStream;
    m_pStream = mate_mixer_context_get_default_input_stream (m_pContext);
    setInputStream(m_pWidget, m_pStream);
}

void UkmediaMainWidget::setInputStream(UkmediaMainWidget *m_pWidget, MateMixerStream *m_pStream)
{
    g_debug("set input stream");
    MateMixerStreamControl *m_pControl = mate_mixer_stream_get_default_control(m_pStream);
    if (m_pControl != nullptr) {
        mate_mixer_stream_control_set_monitor_enabled (m_pControl, FALSE);
    }
    ukuiBarSetStream (m_pWidget, m_pStream);

    if (m_pStream != nullptr) {
        const GList *m_pControls;
        m_pControls = mate_mixer_context_list_stored_controls (m_pWidget->m_pContext);

        /* Move all stored controls to the newly selected default stream */
        while (m_pControls != nullptr) {
            MateMixerStream *parent;

            m_pControl = MATE_MIXER_STREAM_CONTROL (m_pControls->data);
            parent  = mate_mixer_stream_control_get_stream (m_pControl);

            /* Prefer streamless controls to stay the way they are, forcing them to
             * a particular owning stream would be wrong for eg. event controls */
            if (parent != nullptr && parent != m_pStream) {
                MateMixerDirection direction = mate_mixer_stream_get_direction (parent);
                if (direction == MATE_MIXER_DIRECTION_INPUT)
                    mate_mixer_stream_control_set_stream (m_pControl, m_pStream);
            }
            m_pControls = m_pControls->next;
        }

        /* Enable/disable the peak level monitor according to mute state */
        g_signal_connect (G_OBJECT (m_pStream),
                          "notify::mute",
                          G_CALLBACK (onStreamControlMuteNotify),
                          m_pWidget);
    }
    m_pControl = mate_mixer_stream_get_default_control(m_pStream);
    if (G_LIKELY (m_pControl != NULL))
        mate_mixer_stream_control_set_monitor_enabled (m_pControl, TRUE);

    m_pControl = mate_mixer_stream_get_default_control(m_pStream);
    updateInputSettings (m_pWidget,m_pControl);
}

/*
    control 静音通知
*/
void UkmediaMainWidget::onStreamControlMuteNotify (MateMixerStreamControl *m_pControl,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pWidget);
    Q_UNUSED(pspec);
    g_debug("on stream control mute notifty");
    /* Stop monitoring the input stream when it gets muted */
    if (mate_mixer_stream_control_get_mute (m_pControl) == TRUE)
        mate_mixer_stream_control_set_monitor_enabled (m_pControl, false);
    else
        mate_mixer_stream_control_set_monitor_enabled (m_pControl, true);
}

/*
    默认输出流通知
*/
void UkmediaMainWidget::onContextDefaultOutputStreamNotify (MateMixerContext *m_pContext,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("on context default output stream notify");
    MateMixerStream *m_pStream;
    m_pStream = mate_mixer_context_get_default_output_stream (m_pContext);
    updateIconOutput(m_pWidget);
    setOutputStream (m_pWidget, m_pStream);
}

/*
    移除存储control
*/
void UkmediaMainWidget::onContextStoredControlRemoved (MateMixerContext *m_pContext,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pContext);
    Q_UNUSED(m_pName);
    g_debug("on context stored control removed");
    if (m_pWidget->m_pAppVolumeList != nullptr) {
        ukuiBarSetStream (m_pWidget, NULL);
    }
}

/*
 * context设置属性
*/
void UkmediaMainWidget::contextSetProperty(UkmediaMainWidget *m_pWidget)//,guint prop_id,const GValue *value,GParamSpec *pspec)
{
    g_debug("context set property");
    setContext(m_pWidget,m_pWidget->m_pContext);
}

/*
    更新输入音量及图标
*/
void UkmediaMainWidget::updateIconInput (UkmediaMainWidget *m_pWidget)
{
    g_debug("update icon input");
    MateMixerStream *m_pStream;
    MateMixerStreamControl *m_pControl = nullptr;
    MateMixerStreamControlFlags flags;
    const gchar *m_pAppId;
    gboolean show = FALSE;
    m_pStream = mate_mixer_context_get_default_input_stream (m_pWidget->m_pContext);
    const GList *m_pInputs =mate_mixer_stream_list_controls(m_pStream);
    m_pControl = mate_mixer_stream_get_default_control(m_pStream);

    //初始化滑动条的值
    int volume = mate_mixer_stream_control_get_volume(m_pControl);
    int value = volume *100 /65536.0+0.5;
    m_pWidget->m_pInputWidget->m_pIpVolumeSlider->setValue(value);
    QString percent = QString::number(value);
    percent.append("%");
    m_pWidget->m_pInputWidget->m_pIpVolumePercentLabel->setText(percent);
    m_pWidget->m_pInputWidget->m_pInputIconBtn->setFocusPolicy(Qt::NoFocus);
    m_pWidget->m_pInputWidget->m_pInputIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");

    const QSize icon_size = QSize(24,24);
    m_pWidget->m_pInputWidget->m_pInputIconBtn->setIconSize(icon_size);
    if (value <= 0) {
        m_pWidget->m_pInputWidget->m_pInputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-mute.svg"));
    }
    else if (value > 0 && value <= 33) {
        m_pWidget->m_pInputWidget->m_pInputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-low.svg"));
    }
    else if (value >33 && value <= 66) {
        m_pWidget->m_pInputWidget->m_pInputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-medium.svg"));
    }
    else {
        m_pWidget->m_pInputWidget->m_pInputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-high.svg"));
    }

    while (m_pInputs != nullptr) {
        MateMixerStreamControl *input = MATE_MIXER_STREAM_CONTROL (m_pInputs->data);
        MateMixerStreamControlRole role = mate_mixer_stream_control_get_role (input);
        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            MateMixerAppInfo *app_info = mate_mixer_stream_control_get_app_info (input);
            m_pAppId = mate_mixer_app_info_get_id (app_info);
            if (m_pAppId == nullptr) {
                /* A recording application which has no
                 * identifier set */
                g_debug ("Found a recording application control %s",
                         mate_mixer_stream_control_get_label (input));

                if G_UNLIKELY (m_pControl == nullptr) {
                    /* In the unlikely case when there is no
                     * default input control, use the application
                     * control for the icon */
                    m_pControl = input;
                }
                show = TRUE;
                break;
            }
            if (strcmp (m_pAppId, "org.mate.VolumeControl") != 0 &&
                    strcmp (m_pAppId, "org.gnome.VolumeControl") != 0 &&
                    strcmp (m_pAppId, "org.PulseAudio.pavucontrol") != 0) {
                g_debug ("Found a recording application %s", m_pAppId);

                if G_UNLIKELY (m_pControl == nullptr)
                    m_pControl = input;

                show = TRUE;
                break;
            }
        }
        m_pInputs = m_pInputs->next;
    }

    if (show == TRUE)
        g_debug ("Input icon enabled");
    else
        g_debug ("There is no recording application, input icon disabled");

    connect(m_pWidget->m_pInputWidget->m_pIpVolumeSlider,&QSlider::valueChanged,[=](int value){
        QString percent;
        if (value <= 0) {
            mate_mixer_stream_control_set_mute(m_pControl,TRUE);
            mate_mixer_stream_control_set_volume(m_pControl,0);
            percent = QString::number(0);
            m_pWidget->m_pInputWidget->m_pInputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-mute.svg"));
        }
        else if (value > 0 && value <= 33) {
            m_pWidget->m_pInputWidget->m_pInputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-low.svg"));
        }
        else if (value >33 && value <= 66) {
            m_pWidget->m_pInputWidget->m_pInputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-medium.svg"));
        }
        else {
            m_pWidget->m_pInputWidget->m_pInputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-high.svg"));
        }
        percent = QString::number(value);
        mate_mixer_stream_control_set_mute(m_pControl,FALSE);
        percent.append("%");
        m_pWidget->m_pInputWidget->m_pIpVolumePercentLabel->setText(percent);
    });
    streamStatusIconSetControl(m_pWidget, m_pControl);

    if (m_pControl != nullptr) {
        g_debug ("Output icon enabled");
    }
    else {
        g_debug ("There is no output stream/control, output icon disabled");
    }
    //开始监听输入等级
    if (show == TRUE) {
        flags = mate_mixer_stream_control_get_flags(m_pControl);
        mate_mixer_stream_control_set_monitor_enabled(m_pControl,true);
        /* Enable level bar only if supported by the control */
        if (flags & MATE_MIXER_STREAM_CONTROL_HAS_MONITOR) {
            //            g_signal_connect (G_OBJECT (control),
            //                              "monitor-value",
            //                              G_CALLBACK (on_stream_control_monitor_value),
            //                              w);
        }
    }
    else if(show == FALSE) {
        qDebug() << "取消监听输入";
        mate_mixer_stream_control_set_monitor_enabled(m_pControl,false);
    }
}

/*
    更新输出音量及图标
*/
void UkmediaMainWidget::updateIconOutput(UkmediaMainWidget *m_pWidget)
{
    g_debug("update icon output");
    MateMixerStream *m_Stream;
    MateMixerStreamControl *m_pControl = nullptr;

    m_Stream = mate_mixer_context_get_default_output_stream (m_pWidget->m_pContext);
    if (m_Stream != nullptr)
        m_pControl = mate_mixer_stream_get_default_control (m_Stream);

    streamStatusIconSetControl(m_pWidget, m_pControl);
    //初始化滑动条的值
    int volume = mate_mixer_stream_control_get_volume(m_pControl);
    int value = volume *100 /65536.0+0.5;
    m_pWidget->m_pOutputWidget->m_pOpVolumeSlider->setValue(value);
    QString percent = QString::number(value);
    percent.append("%");
    m_pWidget->m_pOutputWidget->m_pOpVolumePercentLabel->setText(percent);
    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setFocusPolicy(Qt::NoFocus);
    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");

    const QSize icon_size = QSize(24,24);
    m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIconSize(icon_size);
    if (value <= 0) {
        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
    }
    else if (value > 0 && value <= 33) {
        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
    }
    else if (value >33 && value <= 66) {

        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
    }
    else {
        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
    }

    //输出音量控制
    //输出滑动条和音量控制
    connect(m_pWidget->m_pOutputWidget->m_pOpVolumeSlider,&QSlider::valueChanged,[=](int value){
        QString percent;

        percent = QString::number(value);
        int volume = value*65536/100;
        mate_mixer_stream_control_set_volume(m_pControl,guint(volume));
        if (value <= 0) {
            mate_mixer_stream_control_set_mute(m_pControl,TRUE);
            mate_mixer_stream_control_set_volume(m_pControl,0);
            percent = QString::number(0);
            m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        }
        else if (value > 0 && value <= 33) {
            m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
        }
        else if (value > 33 && value <= 66) {
            m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
        }
        else {
            m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
        }
        mate_mixer_stream_control_set_mute(m_pControl,FALSE);
        percent.append("%");
        m_pWidget->m_pOutputWidget->m_pOpVolumePercentLabel->setText(percent);
    });
    if (m_pControl != nullptr) {
        g_debug ("Output icon enabled");
    }
    else {
        g_debug ("There is no output stream/control, output icon disabled");
    }
}

void UkmediaMainWidget::streamStatusIconSetControl(UkmediaMainWidget *m_pWidget,MateMixerStreamControl *m_pControl)
{
    g_debug("stream status icon set control");
    g_signal_connect ( G_OBJECT (m_pControl),
                      "notify::volume",
                      G_CALLBACK (onStreamControlVolumeNotify),
                      m_pWidget);
    g_signal_connect (G_OBJECT (m_pControl),
                      "notify::mute",
                      G_CALLBACK (onStreamControlMuteNotify),
                      m_pWidget);

    MateMixerStreamControlFlags flags = mate_mixer_stream_control_get_flags(m_pControl);
    if (flags & MATE_MIXER_STREAM_CONTROL_MUTE_READABLE) {
        g_signal_connect (G_OBJECT (m_pControl),
                          "notify::mute",
                          G_CALLBACK (onControlMuteNotify),
                          m_pWidget);
    }
}

/*
    静音通知
*/
void UkmediaMainWidget::onControlMuteNotify (MateMixerStreamControl *m_pControl,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("on control mute notify");
    gboolean mute = mate_mixer_stream_control_get_mute (m_pControl);
    int volume = int(mate_mixer_stream_control_get_volume(m_pControl));
    volume = int(volume*100/65536.0+0.5);
    if (mute) {
        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
    }
    else if (volume <= 0) {
        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
    }
    else if (volume > 0 && volume <= 33) {
        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
    }
    else if (volume >33 && volume <= 66) {
        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
    }
    else {
        m_pWidget->m_pOutputWidget->m_pOutputIconBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
    }
}

/*
    stream control 声音通知
*/
void UkmediaMainWidget::onStreamControlVolumeNotify (MateMixerStreamControl *m_pControl,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("on stream control volume notify");
    MateMixerStreamControlFlags flags;
    guint volume = 0;
    QString decscription;

    if (m_pControl != nullptr)
        flags = mate_mixer_stream_control_get_flags(m_pControl);

    if (flags&MATE_MIXER_STREAM_CONTROL_VOLUME_READABLE) {
        volume = mate_mixer_stream_control_get_volume(m_pControl);
    }

    decscription = mate_mixer_stream_control_get_label(m_pControl);

    MateMixerStream *m_pStream = mate_mixer_stream_control_get_stream(m_pControl);
    MateMixerDirection direction = mate_mixer_stream_get_direction(m_pStream);

    //设置输出滑动条的值
    int value = volume*100/65536.0 + 0.5;
    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {

        m_pWidget->m_pOutputWidget->m_pOpVolumeSlider->setValue(value);
    }
    else if (direction == MATE_MIXER_DIRECTION_INPUT) {
        m_pWidget->m_pInputWidget->m_pIpVolumeSlider->setValue(value);
    }
}

/*
    设置平衡属性
*/
void UkmediaMainWidget::ukuiBalanceBarSetProperty (UkmediaMainWidget *m_pWidget,MateMixerStreamControl *m_pControl)
{
    g_debug("ukui balance bar set property");
    ukuiBalanceBarSetControl(m_pWidget,m_pControl);
}

/*
    平衡设置control
*/
void UkmediaMainWidget::ukuiBalanceBarSetControl (UkmediaMainWidget *m_pWidget, MateMixerStreamControl *m_pControl)
{
    g_debug("ukui balance bar set control");
    g_signal_connect (G_OBJECT (m_pControl),
                      "notify::balance",
                      G_CALLBACK (onBalanceValueChanged),
                      m_pWidget);
}

/*
    平衡值改变
*/
void UkmediaMainWidget::onBalanceValueChanged (MateMixerStreamControl *m_pControl,GParamSpec *pspec,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(pspec);
    g_debug("on balance value changed");
    gdouble value = mate_mixer_stream_control_get_balance(m_pControl);
    m_pWidget->m_pOutputWidget->m_pOpBalanceSlider->setValue(value*100);
}

/*
    更新输出设置
*/
void UkmediaMainWidget::updateOutputSettings (UkmediaMainWidget *m_pWidget,MateMixerStreamControl *m_pControl)
{
    g_debug("update output settings");
    MateMixerStreamControlFlags flags;
    flags = mate_mixer_stream_control_get_flags(m_pControl);

    if (flags & MATE_MIXER_STREAM_CONTROL_CAN_BALANCE) {
        ukuiBalanceBarSetProperty(m_pWidget,m_pControl);
    }

    connect(m_pWidget->m_pOutputWidget->m_pOpBalanceSlider,&QSlider::valueChanged,[=](int volume){
        gdouble value = volume/100.0;
        mate_mixer_stream_control_set_balance(m_pControl,value);
    });
}

void UkmediaMainWidget::onKeyChanged (GSettings *settings,gchar *key,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(settings);
    g_debug("on key changed");
    if (!strcmp (key, EVENT_SOUNDS_KEY) ||
        !strcmp (key, SOUND_THEME_KEY) ||
        !strcmp (key, INPUT_SOUNDS_KEY)) {
        updateTheme (m_pWidget);
    }
}

/*
    更新主题
*/
void UkmediaMainWidget::updateTheme (UkmediaMainWidget *m_pWidget)
{
    g_debug("update theme");
    char *m_pThemeName;
    gboolean eventsEnabled;
    eventsEnabled = g_settings_get_boolean (m_pWidget->m_pSoundSettings, EVENT_SOUNDS_KEY);
    if (eventsEnabled) {
        m_pThemeName = g_settings_get_string (m_pWidget->m_pSoundSettings, SOUND_THEME_KEY);
    } else {
        m_pThemeName = g_strdup (NO_SOUNDS_THEME_NAME);
    }
    //设置combox的主题
    setComboxForThemeName (m_pWidget, m_pThemeName);
    updateAlertsFromThemeName (m_pWidget, m_pThemeName);
}

/*
    设置主题名到combox
*/
void UkmediaMainWidget::setupThemeSelector (UkmediaMainWidget *m_pWidget)
{
    g_debug("setup theme selector");
    GHashTable  *hash;
    const char * const *dataDirs;
    const char *m_pDataDir;
    char *dir;
    guint i;

    /* Add the theme names and their display name to a hash table,
     * makes it easy to avoid duplicate themes */
    hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

    dataDirs = g_get_system_data_dirs ();
    for (i = 0; dataDirs[i] != NULL; i++) {
        dir = g_build_filename (dataDirs[i], "sounds", NULL);
        soundThemeInDir (m_pWidget,hash, dir);
    }

    m_pDataDir = g_get_user_data_dir ();
    dir = g_build_filename (m_pDataDir, "sounds", NULL);
    soundThemeInDir (m_pWidget,hash, dir);

    /* If there isn't at least one theme, make everything
     * insensitive, LAME! */
    if (g_hash_table_size (hash) == 0) {
        g_warning ("Bad setup, install the freedesktop sound theme");
        g_hash_table_destroy (hash);
        return;
    }
    /* Add the themes to a combobox */
    g_hash_table_destroy (hash);
}

/*
    主题名所在目录
*/
void UkmediaMainWidget::soundThemeInDir (UkmediaMainWidget *m_pWidget,GHashTable *hash,const char *dir)
{
    Q_UNUSED(hash);
    g_debug("sound theme in dir");
    GDir *d;
    const char *m_pName;
    d = g_dir_open (dir, 0, NULL);
    if (d == NULL) {
        return;
    }
    while ((m_pName = g_dir_read_name (d)) != NULL) {
        char *m_pDirName, *m_pIndex, *m_pIndexName;
        /* Look for directories */
        m_pDirName = g_build_filename (dir, m_pName, NULL);
        if (g_file_test (m_pDirName, G_FILE_TEST_IS_DIR) == FALSE) {
            continue;
        }

        /* Look for index files */
        m_pIndex = g_build_filename (m_pDirName, "index.theme", NULL);

        /* Check the name of the theme in the index.theme file */
        m_pIndexName = loadIndexThemeName (m_pIndex, NULL);
        if (m_pIndexName == NULL) {
            continue;
        }
        //设置主题到combox中
        m_pWidget->m_pThemeDisplayNameList->append(m_pIndexName);
        m_pWidget->m_pThemeNameList->append(m_pName);
        m_pWidget->m_pSoundWidget->m_pSoundThemeCombobox->addItem(m_pIndexName);
    }
    g_dir_close (d);
}

/*
    加载下标的主题名
*/
char *UkmediaMainWidget::loadIndexThemeName (const char *index,char **parent)
{
    g_debug("load index theme name");
    GKeyFile *file;
    char *indexname = NULL;
    gboolean hidden;

    file = g_key_file_new ();
    if (g_key_file_load_from_file (file, index, G_KEY_FILE_KEEP_TRANSLATIONS, NULL) == FALSE) {
        g_key_file_free (file);
        return NULL;
    }
    /* Don't add hidden themes to the list */
    hidden = g_key_file_get_boolean (file, "Sound Theme", "Hidden", NULL);
    if (!hidden) {
        indexname = g_key_file_get_locale_string (file,"Sound Theme","Name",NULL,NULL);
        /* Save the parent theme, if there's one */
        if (parent != NULL) {
            *parent = g_key_file_get_string (file,"Sound Theme","Inherits",NULL);
        }
    }
    g_key_file_free (file);
    return indexname;
}

/*
    设置combox的主题名
*/
void UkmediaMainWidget::setComboxForThemeName (UkmediaMainWidget *m_pWidget,const char *name)
{
    g_debug("set combox for theme name");
    gboolean      found;
    static int count = 0;
    /* If the name is empty, use "freedesktop" */
    if (name == NULL || *name == '\0') {
        name = "freedesktop";
    }
    QString value;
    while(!found) {
        value = m_pWidget->m_pThemeNameList->at(count);
        found = (value != "" && value == name);
        count++;
        if( count >= m_pWidget->m_pThemeNameList->size() || found) {
            count = 0;
            break;
        }
    }
    /* When we can't find the theme we need to set, try to set the default
     * one "freedesktop" */
    if (found) {
        m_pWidget->m_pSoundWidget->m_pSoundThemeCombobox->setCurrentText(value);
    } else if (strcmp (name, "freedesktop") != 0) {//设置为默认的主题
        g_debug ("not found, falling back to fdo");
        setComboxForThemeName (m_pWidget, "freedesktop");
    }
}

/*
    更新报警音
*/
void UkmediaMainWidget::updateAlertsFromThemeName (UkmediaMainWidget *m_pWidget,const gchar *m_pName)
{
    g_debug("update alerts from theme name");
    if (strcmp (m_pName, CUSTOM_THEME_NAME) != 0) {
            /* reset alert to default */
        updateAlert (m_pWidget, DEFAULT_ALERT_ID);
    } else {
        int   sound_type;
        char *linkname;
        linkname = NULL;
        sound_type = getFileType ("bell-terminal", &linkname);
        g_debug ("Found link: %s", linkname);
        if (sound_type == SOUND_TYPE_CUSTOM) {
            updateAlert (m_pWidget, linkname);
        }
    }
}

/*
    更新报警声音
*/
void UkmediaMainWidget::updateAlert (UkmediaMainWidget *m_pWidget,const char *alertId)
{
    Q_UNUSED(alertId)
    g_debug("update alert");
    QString theme;
    char *parent;
//    gboolean is_custom;
//    gboolean is_default;
    gboolean add_custom;
    gboolean remove_custom;
    /* Get the current theme's name, and set the parent */
    theme = m_pWidget->m_pSoundWidget->m_pSoundThemeCombobox->currentText();
//    is_custom = (theme == CUSTOM_THEME_NAME) ;
//    is_default = strcmp (alert_id, DEFAULT_ALERT_ID) == 0;

    if (add_custom) {
        setComboxForThemeName (m_pWidget, CUSTOM_THEME_NAME);
    } else if (remove_custom) {
        setComboxForThemeName (m_pWidget, parent);
    }
}

/*
    获取声音文件类型
*/
int UkmediaMainWidget::getFileType (const char *sound_name,char **linked_name)
{
    g_debug("get file type");
    char *name, *filename;
    *linked_name = NULL;
    name = g_strdup_printf ("%s.disabled", sound_name);
    filename = customThemeDirPath (name);
    if (g_file_test (filename, G_FILE_TEST_IS_REGULAR) != FALSE) {
        return SOUND_TYPE_OFF;
    }
    /* We only check for .ogg files because those are the
     * only ones we create */
    name = g_strdup_printf ("%s.ogg", sound_name);
    filename = customThemeDirPath (name);
    g_free (name);

    if (g_file_test (filename, G_FILE_TEST_IS_SYMLINK) != FALSE) {
        *linked_name = g_file_read_link (filename, NULL);
        g_free (filename);
        return SOUND_TYPE_CUSTOM;
    }
    g_free (filename);
    return SOUND_TYPE_BUILTIN;
}

/*
    自定义主题路径
*/
char *UkmediaMainWidget::customThemeDirPath (const char *child)
{
    g_debug("custom theme dir path");
    static char *dir = NULL;
    const char *data_dir;

    if (dir == NULL) {
        data_dir = g_get_user_data_dir ();
        dir = g_build_filename (data_dir, "sounds", CUSTOM_THEME_NAME, NULL);
    }
    if (child == NULL)
        return g_strdup (dir);

    return g_build_filename (dir, child, NULL);
}

/*
    获取报警声音文件的路径
*/
void UkmediaMainWidget::populateModelFromDir (UkmediaMainWidget *m_pWidget,const char *dirname)//从目录查找报警声音文件
{
    g_debug("populate model from dir");
    GDir *d;
    const char *name;
    d = g_dir_open (dirname, 0, NULL);
    if (d == NULL) {
        return;
    }
    while ((name = g_dir_read_name (d)) != NULL) {
        char *path;

        if (! g_str_has_suffix (name, ".xml")) {
            continue;
        }
        path = g_build_filename (dirname, name, NULL);
        populateModelFromFile (m_pWidget, path);
        g_free (path);
    }
    g_dir_close (d);
}

/*
    获取报警声音文件
*/
void UkmediaMainWidget::populateModelFromFile (UkmediaMainWidget *m_pWidget,const char *filename)
{
    g_debug("populate model from file");
    xmlDocPtr  doc;
    xmlNodePtr root;
    xmlNodePtr child;
    gboolean   exists;

    exists = g_file_test (filename, G_FILE_TEST_EXISTS);
    if (! exists) {
        return;
    }
    doc = xmlParseFile (filename);
    if (doc == NULL) {
        return;
    }
    root = xmlDocGetRootElement (doc);
    for (child = root->children; child; child = child->next) {
        if (xmlNodeIsText (child)) {
                continue;
        }
        if (xmlStrcmp (child->name, GVC_SOUND_SOUND) != 0) {
                continue;
        }
        populateModelFromNode (m_pWidget, child);
    }
    xmlFreeDoc (doc);
}

/*
    从节点查找声音文件并加载到组合框中
*/
void UkmediaMainWidget::populateModelFromNode (UkmediaMainWidget *m_pWidget,xmlNodePtr node)
{
    g_debug("populate model from node");
    xmlNodePtr child;
    xmlChar   *filename;
    xmlChar   *name;

    filename = NULL;
    name = xmlGetAndTrimNames (node);
    for (child = node->children; child; child = child->next) {
        if (xmlNodeIsText (child)) {
            continue;
        }

        if (xmlStrcmp (child->name, GVC_SOUND_FILENAME) == 0) {
            filename = xmlNodeGetContent (child);
        } else if (xmlStrcmp (child->name, GVC_SOUND_NAME) == 0) {
                /* EH? should have been trimmed */
        }
    }

    //将找到的声音文件名设置到combox中
    if (filename != NULL && name != NULL) {
        m_pWidget->m_pSoundList->append((const char *)filename);
        m_pWidget->m_pSoundWidget->m_pShutdownCombobox->addItem((char *)name);
        m_pWidget->m_pSoundWidget->m_pLagoutCombobox->addItem((char *)name);
    }
    xmlFree (filename);
    xmlFree (name);
}

/* Adapted from yelp-toc-pager.c */
xmlChar *UkmediaMainWidget::xmlGetAndTrimNames (xmlNodePtr node)
{
    g_debug("xml get and trim names");
    xmlNodePtr cur;
    xmlChar *keep_lang = NULL;
    xmlChar *value;
    int j, keep_pri = INT_MAX;
    const gchar * const * langs = g_get_language_names ();

    value = NULL;
    for (cur = node->children; cur; cur = cur->next) {
        if (! xmlStrcmp (cur->name, GVC_SOUND_NAME)) {
            xmlChar *cur_lang = NULL;
            int cur_pri = INT_MAX;
            cur_lang = xmlNodeGetLang (cur);

            if (cur_lang) {
                for (j = 0; langs[j]; j++) {
                    if (g_str_equal (cur_lang, langs[j])) {
                        cur_pri = j;
                        break;
                    }
                }
            } else {
                cur_pri = INT_MAX - 1;
            }

            if (cur_pri <= keep_pri) {
                if (keep_lang)
                    xmlFree (keep_lang);
                if (value)
                    xmlFree (value);

                value = xmlNodeGetContent (cur);

                keep_lang = cur_lang;
                keep_pri = cur_pri;
            } else {
                if (cur_lang)
                    xmlFree (cur_lang);
            }
        }
    }

    /* Delete all GVC_SOUND_NAME nodes */
    cur = node->children;
    while (cur) {
            xmlNodePtr p_this = cur;
            cur = cur->next;
            if (! xmlStrcmp (p_this->name, GVC_SOUND_NAME)) {
                xmlUnlinkNode (p_this);
                xmlFreeNode (p_this);
            }
    }
    return value;
}

/*
 * 播放报警声音
*/
void UkmediaMainWidget::playAlretSoundFromPath (QString path)
{
    g_debug("play alert sound from path");
   QMediaPlayer *player = new QMediaPlayer;
   player->setMedia(QUrl::fromLocalFile(path));
   player->play();
   qDebug() << path << player->state() << player->mediaStatus();
   player->deleteLater();
   connect(player,&QMediaPlayer::stateChanged,[=](QMediaPlayer::State state){
       qDebug() << "_______________";
//
        switch (state) {
        case QMediaPlayer::StoppedState:
            qDebug() << "状态" << "QMediaPlayer::StoppedState";
            break;
        case QMediaPlayer::PlayingState:
            qDebug() << "状态" << "QMediaPlayer::PlayingState";
            break;
        default:
            qDebug() << "状态" << "QMediaPlayer::PausedState";
            break;
        }
        qDebug() << "play state is " << state;
        player->deleteLater() ;
   });
}

/*
    点击combox播放声音
*/
void UkmediaMainWidget::comboxIndexChangedSlot(int index)
{
    g_debug("combox index changed slot");
    QString sound_name = m_pSoundList->at(index);
    qDebug() << "play sound";
    playAlretSoundFromPath(sound_name);
}

/*
    点击声音主题实现主题切换
*/
void UkmediaMainWidget::themeComboxIndexChangedSlot(int index)
{
    Q_UNUSED(index);
    g_debug("theme combox index changed slot");
    //设置系统主题
}

/*
    点击输出设备combox切换设备
*/
void UkmediaMainWidget::outputDeviceComboxIndexChangedSlot(int index)
{
    g_debug("output device combox index changed slot");
    MateMixerBackendFlags flags;
    QString name = m_pOutputStreamList->at(index);
    //QString转换为const char *
    const char *device_name = name.toLocal8Bit();
    qDebug() << "shebei ming " << name;
    MateMixerStream *m_pStream = mate_mixer_context_get_stream(m_pContext,device_name);

    flags = mate_mixer_context_get_backend_flags (m_pContext);

    if (flags & MATE_MIXER_BACKEND_CAN_SET_DEFAULT_OUTPUT_STREAM) {
        mate_mixer_context_set_default_output_stream (m_pContext, m_pStream);
    }
    else
        setOutputStream (this, m_pStream);
}

/*
    点击输出设备combox切换
*/
void UkmediaMainWidget::inputDeviceComboxIndexChangedSlot(int index)
{
    g_debug("input device combox index changed slot");
    MateMixerBackendFlags flags;
    QString name = m_pInputStreamList->at(index);
    //QString转换为const char *
    const char *m_pDeviceName = name.toLocal8Bit();

    MateMixerStream *m_pStream = mate_mixer_context_get_stream(m_pContext,m_pDeviceName);
    flags = mate_mixer_context_get_backend_flags (m_pContext);

    if (flags & MATE_MIXER_BACKEND_CAN_SET_DEFAULT_INPUT_STREAM) {
        mate_mixer_context_set_default_input_stream (m_pContext, m_pStream);
    }
    else
        setInputStream (this, m_pStream);
}

void UkmediaMainWidget::setOutputStream (UkmediaMainWidget *m_pWidget, MateMixerStream *m_pStream)
{
    g_debug("set output stream");
    MateMixerStreamControl *m_pControl;
    ukuiBarSetStream(m_pWidget,m_pStream);
    if (m_pStream != NULL) {
        const GList *controls;
        controls = mate_mixer_context_list_stored_controls (m_pWidget->m_pContext);
        /* Move all stored controls to the newly selected default stream */
        while (controls != NULL) {
            MateMixerStream        *parent;
            MateMixerStreamControl *m_pControl;
            m_pControl = MATE_MIXER_STREAM_CONTROL (controls->data);
            parent  = mate_mixer_stream_control_get_stream (m_pControl);

            /* Prefer streamless controls to stay the way they are, forcing them to
                 * a particular owning stream would be wrong for eg. event controls */
            if (parent != NULL && parent != m_pStream) {
                MateMixerDirection direction = mate_mixer_stream_get_direction (parent);

                if (direction == MATE_MIXER_DIRECTION_OUTPUT)
                    mate_mixer_stream_control_set_stream (m_pControl, m_pStream);
            }
            controls = controls->next;
        }
    }
    updateOutputStreamList (m_pWidget, m_pStream);
    updateOutputSettings(m_pWidget,m_pControl);
}

/*
    更新输出stream 列表
*/
void UkmediaMainWidget::updateOutputStreamList(UkmediaMainWidget *m_pWidget,MateMixerStream *m_pStream)
{
    g_debug("update output stream list");
    const gchar *m_pName = NULL;
    if (m_pStream != nullptr) {
        m_pName = mate_mixer_stream_get_name(m_pStream);
        m_pWidget->m_pOutputStreamList->append(m_pName);
        qDebug() << "更新输出stream名为" << m_pName;
    }
}

/*
    bar设置stream
*/
void UkmediaMainWidget::ukuiBarSetStream (UkmediaMainWidget  *w,MateMixerStream *m_pStream)
{
    g_debug("ukui bar set stream");
    MateMixerStreamControl *m_pControl = NULL;

    if (m_pStream != NULL)
        m_pControl = mate_mixer_stream_get_default_control (m_pStream);
    ukuiBarSetStreamControl (w, m_pControl);
}

void UkmediaMainWidget::ukuiBarSetStreamControl (UkmediaMainWidget *m_pWidget,MateMixerStreamControl *m_pControl)
{
    Q_UNUSED(m_pWidget);
    g_debug("ukui bar set stream control");
    const gchar *m_pName;
    if (m_pControl != NULL) {
        m_pName = mate_mixer_stream_control_get_name (m_pControl);
    }
}


void UkmediaMainWidget::ukuiInputLevelSetProperty (UkmediaMainWidget *m_pWidget)
{
    g_debug("ukui input level set property");
    scale = GVC_LEVEL_SCALE_LINEAR;
    ukuiInputLevelSetScale (m_pWidget, m_pWidget->scale);
}

void UkmediaMainWidget::ukuiInputLevelSetScale (UkmediaMainWidget *m_pWidget, LevelScale scale)
{
    g_debug("ukui input level set scale");
    if (scale != m_pWidget->scale) {
        ukuiUpdatePeakValue (m_pWidget);
    }
}

void UkmediaMainWidget::ukuiUpdatePeakValue (UkmediaMainWidget *m_pWidget)
{
    g_debug("ukui update peak value");
    gdouble value = ukuiFractionFromAdjustment(m_pWidget);
    m_pWidget->peakFraction = value;

    if (value > m_pWidget->maxPeak) {
        if (m_pWidget->maxPeakId > 0)
            g_source_remove (m_pWidget->maxPeakId);
        m_pWidget->maxPeak = value;
    }
}


void UkmediaMainWidget::inputLevelValueChangedSlot()
{
    g_debug("input level value changed slot");
    ukuiUpdatePeakValue(this);
}

gdouble UkmediaMainWidget::ukuiFractionFromAdjustment (UkmediaMainWidget *m_pWidget)
{
    g_debug("ukui fraction from adjustment");
    gdouble level;
    gdouble fraction = 0.0;
    gdouble min;
    gdouble max;

    level = m_pWidget->m_pInputWidget->m_pInputLevelSlider->value();
    min = m_pWidget->m_pInputWidget->m_pInputLevelSlider->minimum();
    max = m_pWidget->m_pInputWidget->m_pInputLevelSlider->maximum();

    switch (m_pWidget->scale) {
    case GVC_LEVEL_SCALE_LINEAR:
            fraction = (level - min) / (max - min);
            break;
    case GVC_LEVEL_SCALE_LOG:
            fraction = log10 ((level - min + 1) / (max - min + 1));
            break;
    }
    return fraction;
}

/*
    更新输入设置w
*/
void UkmediaMainWidget::updateInputSettings (UkmediaMainWidget *m_pWidget,MateMixerStreamControl *m_pControl)
{
    g_debug ("updating input settings");
    MateMixerStream            *stream;
    MateMixerStreamControlFlags flags;

    /* Get the control currently associated with the input slider */
    if (m_pControl == nullptr)
        return;

    flags = mate_mixer_stream_control_get_flags (m_pControl);

    /* Enable level bar only if supported by the control */
    if (flags & MATE_MIXER_STREAM_CONTROL_HAS_MONITOR) {
        g_signal_connect (G_OBJECT (m_pControl),
                          "monitor-value",
                          G_CALLBACK (onStreamControlMonitorValue),
                          m_pWidget);
    }

    /* Get owning stream of the control */
    stream = mate_mixer_stream_control_get_stream (m_pControl);
    if (G_UNLIKELY (stream == NULL))
        return;
}

void UkmediaMainWidget::onStreamControlMonitorValue (MateMixerStream *m_pStream,gdouble value,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pStream);
    g_debug("on stream control monitor value");
    value = value*100;
    if (value >= 0)
        m_pWidget->m_pInputWidget->m_pInputLevelSlider->setValue(value);
    else
        m_pWidget->m_pInputWidget->m_pInputLevelSlider->setValue(0);
}

/*
    输入stream control add
*/
void UkmediaMainWidget::onInputStreamControlAdded (MateMixerStream *m_pStream,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    g_debug("on input stream control added");
    MateMixerStreamControl *m_pControl;
    m_pControl = mate_mixer_stream_get_control (m_pStream, m_pName);
    if G_LIKELY (m_pControl != NULL) {
        MateMixerStreamControlRole role = mate_mixer_stream_control_get_role (m_pControl);

        /* Non-application input control doesn't affect the icon */
        if (role != MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            return;
        }
    }

    /* Either an application control has been added or we couldn't
     * read the control, this shouldn't happen but let's revalidate the
     * icon to be sure if it does */
    updateIconInput (m_pWidget);
}

/*
    输入stream control removed
*/
void UkmediaMainWidget::onInputStreamControlRemoved (MateMixerStream *m_pStream,const gchar *m_pName,UkmediaMainWidget *m_pWidget)
{
    Q_UNUSED(m_pStream);
    Q_UNUSED(m_pName);
    g_debug("on input stream control removed");
    updateIconInput (m_pWidget);
}

/*
    更新默认的输入stream
*/
gboolean UkmediaMainWidget::updateDefaultInputStream (UkmediaMainWidget *m_pWidget)
{
    g_debug("update default input stream");
    MateMixerStream *m_pStream;
    m_pStream = mate_mixer_context_get_default_input_stream (m_pWidget->m_pContext);

    m_pWidget->m_pInput = (m_pStream == nullptr) ? nullptr : m_pStream;
    if (m_pWidget->m_pInput != nullptr) {
        g_signal_connect (G_OBJECT (m_pWidget->m_pInput),
                          "control-added",
                          G_CALLBACK (onInputStreamControlAdded),
                          m_pWidget);
        g_signal_connect (G_OBJECT (m_pWidget->m_pInput),
                          "control-removed",
                          G_CALLBACK (onInputStreamControlRemoved),
                          m_pWidget);
    }

    /* Return TRUE if the default input stream has changed */
    return TRUE;
}

UkmediaMainWidget::~UkmediaMainWidget()
{
//    delete player;
}
