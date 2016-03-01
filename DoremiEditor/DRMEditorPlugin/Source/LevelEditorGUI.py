from copy import *
import time
import pymel.core as pm
import pymel.core.datatypes as dtype
import maya.cmds as cmds
import maya.mel as mel
import os.path as os

from maya import OpenMayaUI as omui
from PySide.QtCore import *
from PySide.QtGui import *
from PySide.QtUiTools import *
from shiboken import wrapInstance
from sys import path as pythonPath

pyPath = 'C:\Users\Joakim\Documents\Visual Studio 2015\Projects\LevelEditorRefactoring\DoremiLevelEditorRefactor\DoremiLevelEditorRefactor'
pythonPath.append('C:\Users\Joakim\Documents\Visual Studio 2015\Projects\LevelEditorRefactoring\DoremiLevelEditorRefactor\DoremiLevelEditorRefactor')


def getMayaWin():
	#get maya window reference
	mayaWinPtr	= omui.MQtUtil.mainWindow()
	mayaWin		= wrapInstance(long(mayaWinPtr), QWidget)

def loadUI(uiName):
	# object to load ui files
	loader = QUiLoader()

	fileDir = ""

	XMLbuffer = None

	for p in pythonPath:
		fname = p + '/' + uiName
		uiFile = QFile(fname)

		if uiFile.exists():
			fileDir = p
			uiFile.open(QFile.ReadOnly)
			XMLbuffer = QByteArray(uiFile.readAll())
			uiFile.close()
			print 'Found UI file path ' + fileDir
			break
		else:
			print 'UI file' + uiName + ' not found ( ' + p + ' )'

	mayaWinPtr	= omui.MQtUtil.mainWindow()
	mayaWin		= wrapInstance(long(mayaWinPtr), QWidget)
	fixXML(XMLbuffer, p)
	qbuff = QBuffer()
	qbuff.open(QBuffer.ReadOnly|QBuffer.WriteOnly)
	qbuff.write(XMLbuffer)
	qbuff.seek(0)
	mayaPtr = getMayaWin()
	ui = loader.load(qbuff,parentWidget =mayaWin)
	#ui.setWindowFlags(Qt.Window) #Window stays on top
	#ui.setWindowFlags(Qt.WindowStaysOnTopHint) #Window stays on top
	ui.path = p
	return ui

def fixXML(qbyteArray, path):
	if path[-1] != '/':
		path = path+'/'
	path = path.replace("/","\\")

	tempArr = QByteArray("<pixmap>" + path + "\\")
	lastPos = qbyteArray.indexOf("<pixmap>", 0)
	while(lastPos!= -1):
		qbyteArray.replace(lastPos,len("<pixmap>"),tempArr)
		lastPos = qbyteArray.indexOf("<pixmap>", lastPos+1)
	return

class UIController(QObject):
	def __init__(self, ui, mainObject):
		self.mainObject = mainObject
		QObject.__init__(self)
		ui.closeButton.clicked.connect(self.closeUI)
		ui.loadPluginButton.clicked.connect(self.runPlugin)
		ui.unloadPluginButton.clicked.connect(self.unloadPlugin)
		ui.loadSceneButton.clicked.connect(self.loadScene)
		ui.unloadSceneButton.clicked.connect(self.unloadScene)
		ui.resetMsgButton.clicked.connect(self.resetMessages)
		ui.actionPluginLocation.triggered.connect(self.setPluginPath)
		ui.runFilemapButton.clicked.connect(self.runFilemaps)
		ui.stopFilemapsButton.clicked.connect(self.stopFilemaps)
		ui.acceptFileNameButton.clicked.connect(self.getFileName)
		ui.checkBoxDebug.stateChanged.connect(self.setDebugStatus)

		ui.checkBoxDebug.setEnabled(False)
		ui.unloadPluginButton.setEnabled(False)
		ui.loadSceneButton.setEnabled(False)
		ui.unloadSceneButton.setEnabled(False)
		ui.resetMsgButton.setEnabled(False)
		#ui.actionPluginLocation.setEnabled(False)
		ui.runFilemapButton.setEnabled(False)
		ui.stopFilemapsButton.setEnabled(False)
		ui.acceptFileNameButton.setEnabled(False)
	

		# disabled options maybe to be added later
		ui.duplicateAttButton.setEnabled(False)
		ui.pasteAttButton.setEnabled(False)
		ui.resetAttButton.setEnabled(False)
		ui.reloadShapeButton.setEnabled(False)
		ui.updateShapeButton.setEnabled(False)

		ui.reloadTransButton.setEnabled(False)
		ui.updateTransButton.setEnabled(False)
		ui.textBoxMeshes.setEnabled(False)
		ui.textBoxLights.setEnabled(False)
		ui.textBoxTransforms.setEnabled(False)
		ui.textBoxMaterials.setEnabled(False)
		ui.textBoxSelected.setEnabled(False)
		


		self.ui = ui
		self.isRunning = self.mainObject.isPluginLoaded()
		#self.ui.setWindowFlags(PySide.QtCore.Qt.WindowStaysOnTop)
		
		self.ui.show()

	def setPluginPath(self):
		path = pm.fileDialog(dm='*.mll')
		self.mainObject.pluginPath = path
		print self.mainObject.pluginPath
		self.mainObject.savePluginFile()
   # def setPluginPath(self):
       # pm.fileDialog()
	def closeUI(self):
	    self.ui.close()
	def showUI(self):
		self.ui.show()
	def runPlugin(self):
		if self.mainObject.pathIsSet == 1:
			pm.loadPlugin(self.mainObject.pluginPath, name="DoremiEditor")
			self.ui.checkBoxDebug.setEnabled(True)
			self.ui.unloadPluginButton.setEnabled(True)
			self.ui.loadSceneButton.setEnabled(True)
			self.ui.unloadSceneButton.setEnabled(True)
			self.ui.resetMsgButton.setEnabled(True)
			self.ui.actionPluginLocation.setEnabled(True)
			self.ui.runFilemapButton.setEnabled(True)
			self.ui.stopFilemapsButton.setEnabled(True)
			self.ui.acceptFileNameButton.setEnabled(True)
			self.isRunning = 1
		else:
			print 'Plugin file path not set!'
	def unloadPlugin(self):
		if self.isRunning == 1:
			pm.unloadPlugin('DoremiEditor')
			self.isRunning = 0
			self.ui.checkBoxDebug.setCheckState(Qt.Unchecked)
			self.ui.checkBoxDebug.setEnabled(False)
			self.ui.unloadPluginButton.setEnabled(False)
			self.ui.loadSceneButton.setEnabled(False)
			self.ui.unloadSceneButton.setEnabled(False)
			self.ui.resetMsgButton.setEnabled(False)
			self.ui.actionPluginLocation.setEnabled(False)
			self.ui.runFilemapButton.setEnabled(False)
			self.ui.stopFilemapsButton.setEnabled(False)
			self.ui.acceptFileNameButton.setEnabled(False)

		else:
			print 'Plugin is not running!'
	def loadScene(self):
		if self.isRunning == 1:
			mel.eval('drmLoadScene')
		else:
			print 'Plugin not loaded!'
	def unloadScene(self):
		if self.isRunning == 1:
			mel.eval('drmUnloadScene')
		else:
			print 'Plugin not loaded!'
	def resetMessages(self):
		if self.isRunning == 1:
			mel.eval('drmResetMessages')
		else:
			print 'Plugin not loaded!'
	def runFilemaps(self):
		mel.eval('drmToggleFilemaps -s true')
	def stopFilemaps(self):
		mel.eval('drmToggleFilemaps -s false')
	def getFileName(self):
		fileNameString = self.ui.textBoxFileName.text()
		if len(fileNameString)>0:
			mel.eval('drmSetFileName -n '+ fileNameString)
		else:
			print '[Editor Warning] Filename not set!'
	def setDebugStatus(self):
		if self.isRunning == 1:  
			if self.ui.checkBoxDebug.isChecked():
				print 'Debug ON'
				mel.eval('drmSetDebug -s true')
			else:
				print 'Debug OFF'
				mel.eval('drmSetDebug -s false')


class MainObject(object):
	def __init__(self):
		self.pathIsSet = 0
		self.pluginPath = "asd"
		self.scriptPath = os.dirname(os.abspath(__file__))
		self.configFilePath = self.scriptPath+'\drmEditorConfig.txt'
		self.readConfig()
		#self.isPluginLoaded()

	def savePluginFile(self):
		f = open(self.configFilePath, 'w')
		f.write(self.pluginPath+'\n')
		#f.write('\n')
		#f.write('wowszass\n')
		#f.write('babbaba')
		f.close()
		self.readConfig()

	def isPluginLoaded(self):
		isloaded = cmds.pluginInfo(self.pluginPath, query=True, loaded=True)
		if isloaded == True:
			print 'Plugin is loaded'
			return 1

		else:
			print 'Plugin is not loaded'
			return 0

	def readConfig(self):
		if os.exists(self.configFilePath):
			file = open(self.configFilePath, 'r')
			line = file.readline()
			if len(line) < 4:
				print "Invalid filename"
			else:
				print "Filename is: "+line	
				self.pluginPath = line.rstrip('\n')
				self.pathIsSet = 1
			print 'File exists'
			
		else:
			print 'File does not exist'
		#f = open('C:\Users\Joakim\Documents\Visual Studio 2015\Projects\LevelEditorRefactoring\DoremiLevelEditorRefactor\DoremiLevelEditorRefactor\drmEditorConfig.txt', 'r')
		#ling = f.readline()
		#print 'pffft '+ling
		#f.close()



mainObj = MainObject()
cont = UIController(loadUI('DoremiEditorUI.ui'), mainObj)



"""waitCursor -state on;								
$ignoreUpdateCallback = true;						
catch(`loadPlugin "C:/Users/Joakim/Documents/GitHub/UD1414LevelEditorMayaPlugin/UD1414_PluginEditor/x64/Debug/UD1414_Ass02_MayaPlugin_JW.mll"`);				
updatePluginUI( "56" );				
$ignoreUpdateCallback = false;						
waitCursor -state off;"""




#from sys import path as pythonPath

#pythonPath.append('C:\Users\Joakim\Documents\Visual Studio 2015\Projects\LevelEditorRefactoring\DoremiLevelEditorRefactor\DoremiLevelEditorRefactor')

#import LevelEditorGUI   
#reload(LevelEditorGUI)