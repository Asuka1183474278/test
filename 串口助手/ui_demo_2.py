from PyQt5.QtWidgets import QMainWindow, QApplication, QHBoxLayout

import pyqtgraph as pg
import numpy as np
import sys


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_SecondWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(800, 600)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.horizontalLayoutWidget = QtWidgets.QWidget(self.centralwidget)
        self.horizontalLayoutWidget.setGeometry(QtCore.QRect(120, 60, 441, 331))
        self.horizontalLayoutWidget.setObjectName("horizontalLayoutWidget")
        self.pyplot = QtWidgets.QHBoxLayout(self.horizontalLayoutWidget)
        self.pyplot.setContentsMargins(0, 0, 0, 0)
        self.pyplot.setObjectName("pyplot")
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 28))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))

class pg_ui(QMainWindow):
    def __init__(self):
        super(pg_ui, self).__init__()
        self.InitUI()
        self.data_y = np.empty(31)
        self.data_x = []
        self.ptr = 0
        self.timer = pg.QtCore.QTimer()
        self.timer.timeout.connect(self.update_data)
        self.timer.start(100)

    def InitUI(self):
        ui = Ui_SecondWindow()
        ui.setupUi(self)

        ########################plot控件创建#####################
        # 全局参数设定
        pg.setConfigOptions(leftButtonPan=True)
        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')

        # 初始化控件
        self.pw = pg.PlotWidget()
        self.pw.setRange(xRange=[-300, 0])
        self.curve = self.pw.plot()
        ########################################################
        plot_display = self.findChild(QHBoxLayout, 'pyplot')
        # self.find()
        # print(plot_display)
        plot_display.addWidget(self.pw)
        # self.addDockWidget(self.pw)
        self.show()
