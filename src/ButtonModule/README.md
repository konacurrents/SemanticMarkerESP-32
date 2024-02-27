# BUTTON_MODULE is generic

This is for those that want to use the DISPLAY


But the other button processors will call this one.. 

WIthout a registration concept, the best we have is the ifdef

But this could be done inside of ButtonModule

#ifdef M5CORE2_MODULE
#elsif defined(M5_ATOM)
   // ATOM_QRCODE_MODULE
	// ATOM_SOCKET_MODULE
#elsif defined(USE_CAMERA_MODULE)
#elsif defined (M5BUTTON_MODULE)
#endif

#ifdef DISPLAY_MODULE
   redraws stuff..


Also this generic code:

#ifdef M5_ATOM
        //! 1.4.24 use the _atomKind (which CAN change)
        switch (getM5ATOMKind_MainModule())
        {
            case ATOM_KIND_M5_SCANNER:
                //! 8.1.23 for the ATOM Lite QRCode Reader
#ifdef ATOM_QRCODE_MODULE
                loop()
					 setup()
					 buttonB ..
#endif

                break;
            case ATOM_KIND_M5_SOCKET:
                //! 12.26.23 for the ATOM Socket Power
#ifdef ATOM_SOCKET_MODULE
                code ...
#endif
                break;
        }
#endif  // not M5_ATOM
