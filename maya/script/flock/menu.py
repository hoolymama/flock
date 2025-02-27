import pymel.core as pm

import os
import glob

# from Flock import window
import importlib

MAYA_PARENT_WINDOW = "MayaWindow"
HOOLY_MENU = "HoolyMenu"


def ensure_hooly_menu():
    menu = next((m for m in pm.lsUI(menus=True) if m.getLabel() == "Hooly"), None)
    if menu:
        return menu
    return pm.menu(HOOLY_MENU, label="Hooly", tearOff=True, parent=MAYA_PARENT_WINDOW)


class FlockMenu(object):
    def __init__(self):

        pm.mel.source("tumbler")

        self.hooly_menu = ensure_hooly_menu()
        pm.setParent(self.hooly_menu, menu=True)
        pm.menuItem(divider=True, dividerLabel="Flock")
        pm.menuItem(label="Robot Window", command=pm.Callback(show_robot_window))

        pm.menuItem(label="Tumbler", subMenu=True)

        pm.menuItem(
            label="Create", annotation="Select particles", command=pm.Callback(create_tumbler)
        )

        pm.menuItem(
            label="Add Field as Impulse",
            annotation="Select a tumbler and one or more fields",
            command=pm.Callback(tumbler_add_field, 1),
        )

        pm.menuItem(
            label="Add GoalWeightPP",
            annotation="Select a tumbler",
            command=pm.Callback(tumbler_add_goal_weight_pp),
        )

        pm.menuItem(
            label="Create Diagnostics",
            annotation="Select a tumbler",
            command=pm.Callback(tumbler_create_diagnostics),
        )


def show_robot_window():
    importlib.reload(window)
    window.RobotWindow()


def create_tumbler():
    pm.mel.eval("tumbler_create()")


def tumbler_add_field(which):
    pm.mel.eval("tumbler_addField({})".format(which))


def tumbler_add_goal_weight_pp():
    pm.mel.eval("tumbler_addGoalWeightPP()")


def tumbler_create_diagnostics():
    pm.mel.eval("tumbler_attachDiagnosticsFromSel()")
