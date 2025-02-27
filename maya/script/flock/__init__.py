import logging
logging.basicConfig(level=logging.WARNING,  format='[%(name)s:%(levelname)-5s:Line:%(lineno)-4d] %(message)s')
logger = logging.getLogger(__name__)

logger.warning("INITIALIZED LOGGER")