#include <Arduino.h>
// Update pin states
void updatePinStates()
{
  static uint16_t lastRelayState = 0;  // Track previous relay states
  uint16_t currentRelayState = 0;
  
  // Build current desired state
  for (int i = 0; i < 16; i++)
  {
    if (sectionStates[i])
    {
      currentRelayState |= (1 << i);
    }
  }
  
  // Check if state has changed
  if (currentRelayState != lastRelayState)
  {
    // Find relays that are turning ON (new 1s)
    uint16_t turningOn = currentRelayState & ~lastRelayState;
    
    // Turn on relays with staggered timing to reduce inrush current
    for (int i = 0; i < 16; i++)
    {
      if (turningOn & (1 << i))
      {
        relayBoard.setRelay(i + 1, true);  // RelayBoard uses 1-16, not 0-15
        //delayMicroseconds(200);  // Small delay between relay activations
      }
    }
    
    // Turn off relays immediately (no power surge concern)
    for (int i = 0; i < 16; i++)
    {
      if (sectionStates[i] == false && (lastRelayState & (1 << i)))
      {
        relayBoard.setRelay(i + 1, false);  // RelayBoard uses 1-16, not 0-15
      }
    }
    
    lastRelayState = currentRelayState;
  }
}
