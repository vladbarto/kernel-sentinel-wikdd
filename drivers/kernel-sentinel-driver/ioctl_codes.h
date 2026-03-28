#pragma once
#ifndef _IOCTL_CODES_H_
#define _IOCTL_CODES_H_

// Copyright BitDefender
// Authors: Andrei-Marius Muntea (amuntea@bitdefender.com)
//		    Radu Portase (rportase@bitdefender.com)
//          Vlad Bartolomei (vlad.bartolomei@outlook.com)

#define SIOCTL_TYPE 40000
#define IOCTL_SIOCTL_METHOD_BUFFERED \
	CTL_CODE(SIOCTL_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SIOCTL_METHOD_BUFFERED_2 \
	CTL_CODE(SIOCTL_TYPE, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SIOCTL_METHOD_BUFFERED_DRIVER_2 \
	CTL_CODE(SIOCTL_TYPE, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_NONE \
	CTL_CODE(SIOCTL_TYPE, 0x991, METHOD_NEITHER, FILE_READ_ACCESS)
#endif //_IOCTL_CODES_H_
