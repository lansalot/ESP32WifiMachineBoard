
// Update pin states
void updatePinStates()
{
  for (int i = 0; i < 16; i++)
  {
    if (sectionStates[i])
    {
      relayBoard.setRelay(i, true);
    }
    else
    {
      relayBoard.setRelay(i, false);
    }
  }
}
