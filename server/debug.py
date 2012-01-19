#
## \file debug.py
## \brief Simple debugging module : allows output to be turned on or off
#

  
Debug_enabled = 0
  
class Debug:
  #
  ## \brief Prints out text based on whether debugging is on or not
  #
  @staticmethod
  def out(string):
    global Debug_enabled
    
    if Debug_enabled == 1:
      print("[debug] " + string)
      
  #
  ## \brief Enables debugging
  #
  @staticmethod
  def enable():
    global Debug_enabled
    
    Debug_enabled = 1
    Debug.out ("Debug enabled")
  
  #
  ## \brief Disables debugging
  #
  @staticmethod
  def disable():
    global Debug_enabled
    
    Debug_enabled = 1
  
