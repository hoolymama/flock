import os
import pymel.mayautils as pyu
import litter.mod as lm

def load():
    d=os.path.dirname(
    	os.path.dirname(os.path.dirname(os.path.realpath(__file__))))

    m = lm.LitterModule( module_dir=d )
    m.load()
