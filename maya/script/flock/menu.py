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

        pm.mel.source("flock.create")
        pm.mel.source("hexapod.create")
        pm.mel.source("fnPP.createNode")
        pm.mel.source("fnPP.insertNode")
        pm.mel.source("jCreateSensors")
        pm.mel.source("jPriority")
        pm.mel.source("priorityGUI")
        pm.mel.source("jCreateSteerRemap")
        pm.mel.source("jCreateHungerState")
        pm.mel.source("tumbler.create")
        pm.mel.source("roller.create")
        pm.mel.source("jUtils")

        self.hooly_menu = ensure_hooly_menu()
        pm.setParent(self.hooly_menu, menu=True)
        pm.menuItem(divider=True, dividerLabel="Artificial Life")

        pm.menuItem(label="Flock", subMenu=True)
        pm.menuItem(
            label="Create",
            annotation="Pick particles",
            command=pm.Callback(create_flock),
        )
        pm.menuItem(
            label="Add Meshes",
            annotation="Pick meshes and a flock",
            command=pm.Callback(add_meshes),
        )
        pm.menuItem(
            label="Set LOD Meshes",
            annotation="Pick LOD sets and a flock",
            command=pm.Callback(set_lod_meshes),
        )
        pm.menuItem(
            label="Add Mesh Choices",
            annotation="Pick choice nodes and a flock",
            command=pm.Callback(add_mesh_choices),
        )
        pm.menuItem(
            label="Add Attribute",
            annotation="Pick a flock",
            command=pm.Callback(add_attribute),
        )
        pm.menuItem(
            label="Extract Heroes",
            annotation="Pick Some Particle components",
            command=pm.Callback(extract_heroes),
        )
        pm.menuItem(
            label="Cull from Camera",
            annotation="Pick Camera and Particles",
            command=pm.Callback(cull_from_camera),
        )

        pm.setParent("..", menu=True)

        pm.menuItem(label="Hexapod", subMenu=True)
        pm.menuItem(
            label="Create",
            annotation="Select particles",
            command=pm.Callback(create_hexapod),
        )
        pm.menuItem(
            label="Connect Rig",
            annotation="Select rig",
            command=pm.Callback(connect_rig),
        )
        pm.menuItem(
            label="Create Locator",
            annotation="Select rig",
            command=pm.Callback(create_locator),
        )

        pm.setParent("..", menu=True)

        pm.menuItem(label="FnPP", subMenu=True)
        pm.menuItem(label="Insert", subMenu=True)

        nodes = [
            "multPP",
            "addPP",
            "lookupPP",
            "rampLookupPP",
            "reversePP",
            "binormalPP",
            "crossPP",
            "orthogonalPP",
            "coordSysComponentPP",
        ]
        nodes.sort()
        for node in nodes:
            pm.menuItem(label=node, command=pm.Callback(insert_node, node))

        pm.setParent("..", menu=True)

        pm.menuItem(label="Create", subMenu=True)

        nodes = pm.pluginInfo("Flock", query=True, dn=True)
        nodes.sort()
        for node in nodes:
            pm.menuItem(label=node, command=pm.Callback(create_node, node))

        pm.setParent("..", menu=True)
        pm.setParent("..", menu=True)

        pm.menuItem(label="Sensor", subMenu=True)
        pm.menuItem(label="Add", subMenu=True)

        nodes = [
            "alignment",
            "approach",
            "axisSensor",
            "cohesion",
            "exposure",
            "separation",
            "ellipsoidSensor",
            "collisionSensor",
            "slotSensor",
            "splineSensor",
            "geometrySensor",
            "jerkSensor",
        ]
        nodes.sort()
        for node in nodes:
            pm.menuItem(
                label=node,
                annotation=f"Add {node} sensor to selected particle object",
                command=pm.Callback(add_sensor, node),
            )

        pm.setParent("..", menu=True)

        pm.menuItem(
            label="Connect Curve to Spline Sensor",
            command=pm.Callback(connect_curve_to_spline_sensor),
        )
        pm.menuItem(
            label="Connect Meshes to Geometry Sensor",
            command=pm.Callback(connect_meshes_to_geometry_sensor),
        )

        pm.setParent("..", menu=True)

        pm.menuItem(label="Priority", subMenu=True)

        pm.menuItem(
            label="Connect To Priority", command=pm.Callback(connect_to_priority)
        )
        pm.menuItem(
            label="Redirect Through Priority",
            command=pm.Callback(redirect_through_priority),
        )

        pm.menuItem(label="Priority GUI", command=pm.Callback(priority_gui))

        pm.setParent("..", menu=True)

        pm.menuItem(label="Steer Remap", subMenu=True)

        pm.menuItem(label="Create Steer Remap", command=pm.Callback(create_steer_remap))
        pm.menuItem(
            label="Create Hunger State", command=pm.Callback(create_hunger_state)
        )

        pm.setParent("..", menu=True)

        pm.menuItem(label="Tumbler", subMenu=True)

        pm.menuItem(label="Create", command=pm.Callback(create_tumbler))
        pm.menuItem(
            label="Add Field as Torque", command=pm.Callback(add_field_as_torque)
        )
        pm.menuItem(
            label="Add Field as Impulse", command=pm.Callback(add_field_as_impulse)
        )
        pm.menuItem(label="Add Goal Weight PP", command=pm.Callback(add_goal_weight_pp))
        pm.menuItem(label="Create Diagnostics", command=pm.Callback(create_diagnostics))

        pm.setParent("..", menu=True)

        pm.menuItem(label="Roller", subMenu=True)

        pm.menuItem(label="Create", command=pm.Callback(create_roller))


# Flock menu items
def create_flock():
    pm.mel.eval("flock.create()")


def add_meshes():
    pm.mel.eval("flock.addMeshes()")


def set_lod_meshes():
    pm.mel.eval("flock.setLODMeshes()")


def add_mesh_choices():
    pm.mel.eval("flock.addMeshChoices()")


def add_attribute():
    pm.mel.eval("flock.addAttribute()")


def extract_heroes():
    pm.mel.eval("flock.extractHeroes()")


def cull_from_camera():
    pm.mel.eval("flock.cullFromCamera()")


# Hexapod menu
def create_hexapod():
    pm.mel.eval("hexapod.create()")


def connect_rig():
    pm.mel.eval("hexapod.connectRig()")


def create_locator():
    pm.mel.eval("hexapod.createLocator()")


# FnPP menu items
def insert_node(node):
    pm.mel.eval(f"fnPP.insert('{node}')")


def create_node(node):
    pm.mel.eval(f"fnPP.create('{node}')")


# Sensor menu items
def add_sensor(node):
    pm.mel.eval(f"sensor.add('{node}')")


def connect_curve_to_spline_sensor():
    pm.mel.eval("sensor.connectCurveToSplineSensor()")


def connect_meshes_to_geometry_sensor():
    pm.mel.eval("sensor.connectMeshToGeometrySensor()")


# Priority menu items
def connect_to_priority():
    pm.mel.eval("jConnectSignalToPriority()")


def redirect_through_priority():
    pm.mel.eval("jRedirectThroughPriority()")


def priority_gui():
    pm.mel.eval("priorityGUI()")


# Steer Remap menu items
def create_steer_remap():
    pm.mel.eval("jCreateSteerRemap()")


def create_hunger_state():
    pm.mel.eval("jCreateHungerState()")


# Tumbler menu items
def create_tumbler():
    pm.mel.eval("tumbler.create()")


def add_field_as_torque():
    pm.mel.eval("tumbler.addField(0)")


def add_field_as_impulse():
    pm.mel.eval("tumbler.addField(1)")


def add_goal_weight_pp():
    pm.mel.eval("tumbler.addGoalWeightPP()")


def create_diagnostics():
    pm.mel.eval("tumbler.attachDiagnosticsFromSel()")


# Roller menu items
def create_roller():
    pm.mel.eval("roller.create()")

