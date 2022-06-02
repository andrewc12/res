				Memecode Res Readme
				-------------------

Disclaimer and License:
	This software is provided as is, no warrentee of fitness for 
	any purpose is given by Memecode. Use it at your own risk.

	Also this software is covered by the LGPL licence available here:
		http://www.gnu.org/copyleft/lesser.html


Usage:
	Res is a little windows command line utility to report the 
	current screen resolution and refresh and optionally change 
	it to something else. If you attempt to change it to the 
	current mode it does nothing, so it can be used in your 
	startup folder.

	The parameters for the command are:
    * Nothing
      Displays the current screen mode(s).
    * -x <width>
      Set the horizontal resolution in pixels
    * -y <height>
      Set the vertical resolution in pixels
    * -bpp <bits_per_pixel>
      Set the number of bits per pixels, valid values are 4, 8, 
      15, 16, 24 or 32 depending on your video card.
    * -refresh <refresh_rate>
      Set the refresh rate in Hz. This is dependant on what your 
      video card and monitor support, do check in the display 
      settings first.
    * -device <device_index>
      This optional parameter sets the device to set. To get 
      the device index first list the devices by running Res 
      with no parameters. If not supplied the primary display 
      will be changed.

Compiling:
	You will need MSVC 6 and the Platform SDK to compile this code.
	Add the lib folder of your SDK install to the linker input paths
	so that your not linking against the MSVC user32.lib which is
	missing a symbol.

(c) 2006 Matthew Allen
fret@memecode.com
http://www.memecode.com/res.php