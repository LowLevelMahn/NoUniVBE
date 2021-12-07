# NoUniVBE

DOS TSR to prevent UniVBE 5.3x loading by faking the original UniVBE behavior. There is absolutely no need for a game running inside dosbox to use the legacy VESA code of UniVBE

How it works:

UniVBE TSR does the following:
  detours interrupt 10h/function ax=4F00h -> VESA Information
  detours interrupt 10h/function ax=4F0Fh -> install detection helper, returns a struct with UniVBE infos (version, license, ...)
  detours interrupt 2Fh -> install detection helper

UniVBE dectection:
  check if interrupt 10h/4F00 got the UniVBE signature in the Oem-String
  check if interrupt 10h/4F0F returns UniVBE infos
  check if interrupt 10h.vector.segment:0 is the UniVBE signature
  check if interrupt 2Fh.vector.segment is equal to interrupt 10h.vector.segment

NoUniVBE mimics this behavior to fake a real installed UniVBE but keep all interrupt calls handled by the real Hardware or dosbox

# Status:
Mimics UniVBE 5.3a TSR "interfaces" - UniVBE think its already installed, in-game linked UniVBE (compatible to 5.3a) is inhibited

Tested with "Street Racer":

Preloading NoUniVBE gives a VGA refresh rate of 70Hz (default under dosbox) instead of 35Hz

# Todos:
Mimic also Version < 5.3a - for example 4.2, 5.0, 5.1 - they use different detection methods
