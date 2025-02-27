# import os
# import litter.mod as lm


# def load():
#     __load_mod__(True)

# def unload():
#     __load_mod__(False)

# def __load_mod__(load=True):
#     d=os.path.dirname(
#             os.path.dirname(
#             os.path.dirname(os.path.realpath(__file__))))

#     m = lm.LitterModule( 
#             module_dir=d, 
#             menu_annotation='<%= settings[:product_description]%>')
            
#     if load:
#         m.load()
#     else:
#         m.unload()

import logging
logging.basicConfig(level=logging.WARNING,  format='[%(name)s:%(levelname)-5s:Line:%(lineno)-4d] %(message)s')
logger = logging.getLogger(__name__)

logger.warning("INITIALIZED LOGGER")