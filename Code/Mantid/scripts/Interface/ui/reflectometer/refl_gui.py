# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'D:\mantid\windows\Code\Mantid\scripts\Interface\ui\reflectometer/refl_gui.ui'
#
# Created: Tue Nov 19 14:30:54 2013
#      by: PyQt4 UI code generator 4.8.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_ref_gui(object):
    def setupUi(self, ref_gui):
        ref_gui.setObjectName(_fromUtf8("ref_gui"))
        ref_gui.resize(1000, 400)
        ref_gui.setAcceptDrops(True)
        self.mainRowLayout = QtGui.QWidget(ref_gui)
        self.mainRowLayout.setObjectName(_fromUtf8("mainRowLayout"))
        self.verticalLayout_4 = QtGui.QVBoxLayout(self.mainRowLayout)
        self.verticalLayout_4.setObjectName(_fromUtf8("verticalLayout_4"))
        self.topRowLayout = QtGui.QHBoxLayout()
        self.topRowLayout.setSpacing(12)
        self.topRowLayout.setObjectName(_fromUtf8("topRowLayout"))
        self.comboInstrument = QtGui.QComboBox(self.mainRowLayout)
        self.comboInstrument.setObjectName(_fromUtf8("comboInstrument"))
        self.topRowLayout.addWidget(self.comboInstrument)
        self.labelRB = QtGui.QLabel(self.mainRowLayout)
        self.labelRB.setObjectName(_fromUtf8("labelRB"))
        self.topRowLayout.addWidget(self.labelRB)
        self.textRB = QtGui.QLineEdit(self.mainRowLayout)
        self.textRB.setMaximumSize(QtCore.QSize(55, 16777215))
        self.textRB.setCursorPosition(0)
        self.textRB.setObjectName(_fromUtf8("textRB"))
        self.topRowLayout.addWidget(self.textRB)
        self.labelRuns = QtGui.QLabel(self.mainRowLayout)
        self.labelRuns.setObjectName(_fromUtf8("labelRuns"))
        self.topRowLayout.addWidget(self.labelRuns)
        self.textRuns = QtGui.QLineEdit(self.mainRowLayout)
        self.textRuns.setMaximumSize(QtCore.QSize(100, 16777215))
        self.textRuns.setObjectName(_fromUtf8("textRuns"))
        self.topRowLayout.addWidget(self.textRuns)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.topRowLayout.addItem(spacerItem)
        self.labelPolarCorrect = QtGui.QLabel(self.mainRowLayout)
        self.labelPolarCorrect.setObjectName(_fromUtf8("labelPolarCorrect"))
        self.topRowLayout.addWidget(self.labelPolarCorrect)
        self.comboPolarCorrect = QtGui.QComboBox(self.mainRowLayout)
        self.comboPolarCorrect.setObjectName(_fromUtf8("comboPolarCorrect"))
        self.topRowLayout.addWidget(self.comboPolarCorrect)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.topRowLayout.addItem(spacerItem1)
        self.checkTickAll = QtGui.QCheckBox(self.mainRowLayout)
        self.checkTickAll.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.checkTickAll.setObjectName(_fromUtf8("checkTickAll"))
        self.topRowLayout.addWidget(self.checkTickAll)
        spacerItem2 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.topRowLayout.addItem(spacerItem2)
        self.buttonAuto = QtGui.QPushButton(self.mainRowLayout)
        self.buttonAuto.setObjectName(_fromUtf8("buttonAuto"))
        self.topRowLayout.addWidget(self.buttonAuto)
        self.verticalLayout_4.addLayout(self.topRowLayout)
        self.bottomRowLayout = QtGui.QHBoxLayout()
        self.bottomRowLayout.setObjectName(_fromUtf8("bottomRowLayout"))
        self.mainList = QtGui.QListWidget(self.mainRowLayout)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.mainList.sizePolicy().hasHeightForWidth())
        self.mainList.setSizePolicy(sizePolicy)
        self.mainList.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection)
        self.mainList.setObjectName(_fromUtf8("mainList"))
        self.bottomRowLayout.addWidget(self.mainList)
        self.buttonTransfer = QtGui.QPushButton(self.mainRowLayout)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.buttonTransfer.sizePolicy().hasHeightForWidth())
        self.buttonTransfer.setSizePolicy(sizePolicy)
        self.buttonTransfer.setMaximumSize(QtCore.QSize(20, 16777215))
        self.buttonTransfer.setObjectName(_fromUtf8("buttonTransfer"))
        self.bottomRowLayout.addWidget(self.buttonTransfer)
        self.tableColumnLayout = QtGui.QVBoxLayout()
        self.tableColumnLayout.setObjectName(_fromUtf8("tableColumnLayout"))
        self.mainTable = QtGui.QTableView(self.mainRowLayout)
        self.mainTable.setObjectName(_fromUtf8("mainTable"))
        self.tableColumnLayout.addWidget(self.mainTable)
        self.tableButtonLayout = QtGui.QHBoxLayout()
        self.tableButtonLayout.setObjectName(_fromUtf8("tableButtonLayout"))
        self.buttonProcess = QtGui.QPushButton(self.mainRowLayout)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.buttonProcess.sizePolicy().hasHeightForWidth())
        self.buttonProcess.setSizePolicy(sizePolicy)
        self.buttonProcess.setObjectName(_fromUtf8("buttonProcess"))
        self.tableButtonLayout.addWidget(self.buttonProcess)
        self.buttonClear = QtGui.QPushButton(self.mainRowLayout)
        self.buttonClear.setObjectName(_fromUtf8("buttonClear"))
        self.tableButtonLayout.addWidget(self.buttonClear)
        self.tableColumnLayout.addLayout(self.tableButtonLayout)
        self.bottomRowLayout.addLayout(self.tableColumnLayout)
        self.verticalLayout_4.addLayout(self.bottomRowLayout)
        ref_gui.setCentralWidget(self.mainRowLayout)
        self.menubar = QtGui.QMenuBar(ref_gui)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1000, 21))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        self.menuFile = QtGui.QMenu(self.menubar)
        self.menuFile.setObjectName(_fromUtf8("menuFile"))
        self.menuHelp = QtGui.QMenu(self.menubar)
        self.menuHelp.setObjectName(_fromUtf8("menuHelp"))
        ref_gui.setMenuBar(self.menubar)
        self.actionSave_Table = QtGui.QAction(ref_gui)
        self.actionSave_Table.setObjectName(_fromUtf8("actionSave_Table"))
        self.actionLoad_Table = QtGui.QAction(ref_gui)
        self.actionLoad_Table.setObjectName(_fromUtf8("actionLoad_Table"))
        self.actionRe_Load_Table = QtGui.QAction(ref_gui)
        self.actionRe_Load_Table.setObjectName(_fromUtf8("actionRe_Load_Table"))
        self.actionSave_Workspaces = QtGui.QAction(ref_gui)
        self.actionSave_Workspaces.setObjectName(_fromUtf8("actionSave_Workspaces"))
        self.actionMantid_Help = QtGui.QAction(ref_gui)
        self.actionMantid_Help.setObjectName(_fromUtf8("actionMantid_Help"))
        self.menuFile.addAction(self.actionSave_Table)
        self.menuFile.addAction(self.actionLoad_Table)
        self.menuFile.addAction(self.actionRe_Load_Table)
        self.menuFile.addAction(self.actionSave_Workspaces)
        self.menuHelp.addAction(self.actionMantid_Help)
        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuHelp.menuAction())

        self.retranslateUi(ref_gui)
        QtCore.QMetaObject.connectSlotsByName(ref_gui)

    def retranslateUi(self, ref_gui):
        ref_gui.setWindowTitle(QtGui.QApplication.translate("ref_gui", "ISIS Reflectometry", None, QtGui.QApplication.UnicodeUTF8))
        self.labelRB.setText(QtGui.QApplication.translate("ref_gui", "RB:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelRuns.setText(QtGui.QApplication.translate("ref_gui", "Transmission run(s):", None, QtGui.QApplication.UnicodeUTF8))
        self.labelPolarCorrect.setText(QtGui.QApplication.translate("ref_gui", "Polarisation corrections", None, QtGui.QApplication.UnicodeUTF8))
        self.checkTickAll.setText(QtGui.QApplication.translate("ref_gui", "(un)tick all", None, QtGui.QApplication.UnicodeUTF8))
        self.buttonAuto.setText(QtGui.QApplication.translate("ref_gui", "AutoFill", None, QtGui.QApplication.UnicodeUTF8))
        self.buttonTransfer.setText(QtGui.QApplication.translate("ref_gui", "=>", None, QtGui.QApplication.UnicodeUTF8))
        self.buttonProcess.setText(QtGui.QApplication.translate("ref_gui", "Process", None, QtGui.QApplication.UnicodeUTF8))
        self.buttonClear.setText(QtGui.QApplication.translate("ref_gui", "Clear all", None, QtGui.QApplication.UnicodeUTF8))
        self.menuFile.setTitle(QtGui.QApplication.translate("ref_gui", "File", None, QtGui.QApplication.UnicodeUTF8))
        self.menuHelp.setTitle(QtGui.QApplication.translate("ref_gui", "Help", None, QtGui.QApplication.UnicodeUTF8))
        self.actionSave_Table.setText(QtGui.QApplication.translate("ref_gui", "Save Table", None, QtGui.QApplication.UnicodeUTF8))
        self.actionSave_Table.setShortcut(QtGui.QApplication.translate("ref_gui", "Ctrl+S", None, QtGui.QApplication.UnicodeUTF8))
        self.actionLoad_Table.setText(QtGui.QApplication.translate("ref_gui", "Load Table", None, QtGui.QApplication.UnicodeUTF8))
        self.actionLoad_Table.setShortcut(QtGui.QApplication.translate("ref_gui", "Ctrl+O", None, QtGui.QApplication.UnicodeUTF8))
        self.actionRe_Load_Table.setText(QtGui.QApplication.translate("ref_gui", "Re-Load Table", None, QtGui.QApplication.UnicodeUTF8))
        self.actionRe_Load_Table.setShortcut(QtGui.QApplication.translate("ref_gui", "Ctrl+R", None, QtGui.QApplication.UnicodeUTF8))
        self.actionSave_Workspaces.setText(QtGui.QApplication.translate("ref_gui", "Save Workspaces", None, QtGui.QApplication.UnicodeUTF8))
        self.actionSave_Workspaces.setShortcut(QtGui.QApplication.translate("ref_gui", "Ctrl+Shift+S", None, QtGui.QApplication.UnicodeUTF8))
        self.actionMantid_Help.setText(QtGui.QApplication.translate("ref_gui", "Mantid Help", None, QtGui.QApplication.UnicodeUTF8))
        self.actionMantid_Help.setShortcut(QtGui.QApplication.translate("ref_gui", "Ctrl+H", None, QtGui.QApplication.UnicodeUTF8))

