/***************************************************************************
                          dlgprefkey.cpp  -  description
                             -------------------
    begin                : Thu Jun 7 2012
    copyright            : (C) 2012 by Keith Salisbury
    email                : keithsalisbury@gmail.com
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <qlineedit.h>
#include <qfiledialog.h>
#include <qwidget.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <QtCore>
#include <QMessageBox>
#include "track/key_preferences.h"

#include "dlgprefkey.h"
#include "xmlparse.h"
#include "controlobject.h"
#include "vamp/vampanalyser.h"
#include "vamp/vamppluginloader.h"

using Vamp::Plugin;
using Vamp::PluginHostAdapter;
using Vamp::HostExt::PluginLoader;
using Vamp::HostExt::PluginWrapper;
using Vamp::HostExt::PluginInputDomainAdapter;

DlgPrefKey::DlgPrefKey(QWidget* parent, ConfigObject<ConfigValue>* _config)
        : QWidget(parent),
          Ui::DlgPrefKeyDlg(),
          m_pconfig(_config) {
    setupUi(this);

    m_selectedAnalyser = "qm-keydetector:2";
    populate();
    loadSettings();

    // Connections
    //connect(plugincombo, SIGNAL(currentIndexChanged(int)),
    //        this, SLOT(pluginSelected(int)));
    connect(banalyserenabled, SIGNAL(stateChanged(int)),
          this, SLOT(analyserEnabled(int)));
    connect(bfastAnalysisEnabled, SIGNAL(stateChanged(int)),
            this, SLOT(fastAnalysisEnabled(int)));
   // connect(reset, SIGNAL(clicked(bool)), this, SLOT(setDefaults()));
    connect(breanalyzeEnabled, SIGNAL(stateChanged(int)),
            this, SLOT(reanalyzeEnabled(int)));
}

DlgPrefKey::~DlgPrefKey() {
}

void DlgPrefKey::loadSettings(){
    //if(m_pconfig->getValueString(
      //  ConfigKey(VAMP_CONFIG_KEY, VAMP_ANALYSER_KEY_PLUGIN_ID))==QString("")) {
        //setDefaults();
           // Write to config file so AnalyzerBeats can get the data
        //return;
    //}

   // QString pluginid = m_pconfig->getValueString(
     //   ConfigKey(VAMP_CONFIG_KEY, VAMP_ANALYSER_KEY_PLUGIN_ID));
    //m_selectedAnalyser = pluginid;

    m_bAnalyserEnabled = static_cast<bool>(m_pconfig->getValueString(
        ConfigKey(KEY_CONFIG_KEY, KEY_DETECTION_ENABLED)).toInt());

    m_bFastAnalysisEnabled = static_cast<bool>(m_pconfig->getValueString(
        ConfigKey(KEY_CONFIG_KEY, KEY_FAST_ANALYSIS)).toInt());

    m_bReanalyzeEnabled = static_cast<bool>(m_pconfig->getValueString(
        ConfigKey(KEY_CONFIG_KEY, KEY_REANALYZE_WHEN_SETTINGS_CHANGE)).toInt());

    slotApply();
  //  if (!m_listIdentifier.contains(pluginid)) {
    //    setDefaults();
    //}

    slotUpdate();
}

void DlgPrefKey::setDefaults() {
    //if (!m_listIdentifier.contains("qm-tempotracker:0")) {
      //  qDebug() << "DlgPrefBeats: qm-tempotracker Vamp plugin not found";
        //return;
    //}
    //m_selectedAnalyser = "qm-tempotracker:0";
    m_bAnalyserEnabled = true;
    m_bFastAnalysisEnabled = false;
    m_bReanalyzeEnabled = false;

    //slotApply();
    slotUpdate();
}

void  DlgPrefKey::analyserEnabled(int i){
    m_bAnalyserEnabled = static_cast<bool>(i);
    slotUpdate();
}

void  DlgPrefKey::fastAnalysisEnabled(int i){
    m_bFastAnalysisEnabled = static_cast<bool>(i);
    slotUpdate();
}

void DlgPrefKey::reanalyzeEnabled(int i){
    m_bReanalyzeEnabled = static_cast<bool>(i);
    slotUpdate();
}

void DlgPrefKey::slotApply() {
    //int selected = m_listIdentifier.indexOf(m_selectedAnalyser);
    //if (selected == -1)
     //   return;

    //m_pconfig->set(ConfigKey(
      //  VAMP_CONFIG_KEY, VAMP_ANALYSER_KEY_LIBRARY), ConfigValue(m_listLibrary[selected]));
    //m_pconfig->set(ConfigKey(
      //  VAMP_CONFIG_KEY, VAMP_ANALYSER_KEY_PLUGIN_ID), ConfigValue(m_selectedAnalyser));
    m_pconfig->set(
        ConfigKey(KEY_CONFIG_KEY, KEY_DETECTION_ENABLED),
        ConfigValue(m_bAnalyserEnabled ? 1 : 0));
    m_pconfig->set(
        ConfigKey(KEY_CONFIG_KEY, KEY_FAST_ANALYSIS),
        ConfigValue(m_bFastAnalysisEnabled ? 1 : 0));
    m_pconfig->set(
        ConfigKey(KEY_CONFIG_KEY, KEY_REANALYZE_WHEN_SETTINGS_CHANGE),
        ConfigValue(m_bReanalyzeEnabled ? 1 : 0));
    m_pconfig->Save();
}

void DlgPrefKey::slotUpdate()
{
    //plugincombo->setEnabled(m_banalyserEnabled);
    banalyserenabled->setChecked(m_bAnalyserEnabled);
    bfastAnalysisEnabled->setChecked(m_bFastAnalysisEnabled);
    breanalyzeEnabled->setChecked(m_bReanalyzeEnabled);
    slotApply();

    //if(!m_banalyserEnabled)
      //  return;

    //int comboselected = m_listIdentifier.indexOf(m_selectedAnalyser);
    //if( comboselected==-1){
      //  qDebug()<<"DlgPrefBeats: Plugin not found in slotUpdate()";
        //return;
    //}

    //plugincombo->setCurrentIndex(comboselected);
}

void DlgPrefKey::populate() {
   VampAnalyser::initializePluginPaths();
   m_listIdentifier.clear();
   m_listName.clear();
   m_listLibrary.clear();
   disconnect(plugincombo, SIGNAL(currentIndexChanged(int)),
              this, SLOT(pluginSelected(int)));
   plugincombo->clear();
   plugincombo->setDuplicatesEnabled(false);
   connect(plugincombo, SIGNAL(currentIndexChanged(int)),
           this, SLOT(pluginSelected(int)));
   VampPluginLoader *loader = VampPluginLoader::getInstance();
   std::vector<PluginLoader::PluginKey> plugins = loader->listPlugins();
   qDebug() << "VampPluginLoader::listPlugins() returned" << plugins.size() << "plugins";
   for (unsigned int iplugin=0; iplugin < plugins.size(); iplugin++) {
       // TODO(XXX): WTF, 48000
       Plugin *plugin = loader->loadPlugin(plugins[iplugin], 48000);
       //TODO: find a way to add key detectors only
       if (plugin) {
           Plugin::OutputList outputs = plugin->getOutputDescriptors();
           for (unsigned int ioutput=0; ioutput < outputs.size(); ioutput++) {
               QString displayname = QString::fromStdString(plugin->getIdentifier()) + ":"
                                           + QString::number(ioutput);
               QString displaynametext = QString::fromStdString(plugin->getName());
               qDebug() << "Plugin output displayname:" << displayname << displaynametext;
               bool goodones = displayname.contains("qm-keydetector:2");

               if (goodones) {
                   m_listName << displaynametext;
                   QString pluginlibrary = QString::fromStdString(plugins[iplugin]).section(":",0,0);
                   m_listLibrary << pluginlibrary;
                   QString displayname = QString::fromStdString(plugin->getIdentifier()) + ":"
                           + QString::number(ioutput);
                   m_listIdentifier << displayname;
                   plugincombo->addItem(displaynametext, displayname);
               }
           }
           delete plugin;
           plugin = 0;
       }
   }
}

