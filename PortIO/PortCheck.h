/*
 *   FILE:  PortCheck.h
 *
 *   Interface to check status of serial and parallel ports.
 *
 *   Copyright © 2002 Anadigm. All rights reserved.
 */

#if !defined(ANADIGM_PORTCHECK_H_INCLUDED)
#define ANADIGM_PORTCHECK_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// Port status flags
enum PortStatus
{
   PORT_AVAILABLE = 0,     // Port is available and ready for use.
   PORT_NOT_FOUND,         // Port not found.
   PORT_IN_USE,            // Port is already in use.
   PORT_STATUS_UNKNOWN,    // Unrecognized port error.
   NUM_OF_PORT_STATUS
};

// Get description of port status
CString GetEnumString(PortStatus nStatus);

/////////////////////////////////////////////////////////////////////////////
// Port types
enum PortType
{
   LPT1 = 0,
   COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9, COM10,
   COM11, COM12, COM13, COM14, COM15, COM16, COM17, COM18, COM19, COM20,
   COM21, COM22, COM23, COM24, COM25, COM26, COM27, COM28, COM29, COM30,
   COM31, COM32, COM33, COM34, COM35, COM36, COM37, COM38, COM39, COM40,
   COM41, COM42, COM43, COM44, COM45, COM46, COM47, COM48, COM49, COM50,
   COM51, COM52, COM53, COM54, COM55, COM56, COM57, COM58, COM59, COM60,
   COM61, COM62, COM63, COM64, COM65, COM66, COM67, COM68, COM69, COM70,
   COM71, COM72, COM73, COM74, COM75, COM76, COM77, COM78, COM79, COM80,
   COM81, COM82, COM83, COM84, COM85, COM86, COM87, COM88, COM89, COM90,
   COM91, COM92, COM93, COM94, COM95, COM96, COM97, COM98, COM99, COM100,
   COM101, COM102, COM103, COM104, COM105, COM106, COM107, COM108, COM109, COM110,
   COM111, COM112, COM113, COM114, COM115, COM116, COM117, COM118, COM119, COM120,
   COM121, COM122, COM123, COM124, COM125, COM126, COM127, COM128, COM129, COM130,
   COM131, COM132, COM133, COM134, COM135, COM136, COM137, COM138, COM139, COM140,
   COM141, COM142, COM143, COM144, COM145, COM146, COM147, COM148, COM149, COM150,
   COM151, COM152, COM153, COM154, COM155, COM156, COM157, COM158, COM159, COM160,
   COM161, COM162, COM163, COM164, COM165, COM166, COM167, COM168, COM169, COM170,
   COM171, COM172, COM173, COM174, COM175, COM176, COM177, COM178, COM179, COM180,
   COM181, COM182, COM183, COM184, COM185, COM186, COM187, COM188, COM189, COM190,
   COM191, COM192, COM193, COM194, COM195, COM196, COM197, COM198, COM199, COM200,
   COM201, COM202, COM203, COM204, COM205, COM206, COM207, COM208, COM209, COM210,
   COM211, COM212, COM213, COM214, COM215, COM216, COM217, COM218, COM219, COM220,
   COM221, COM222, COM223, COM224, COM225, COM226, COM227, COM228, COM229, COM230,
   COM231, COM232, COM233, COM234, COM235, COM236, COM237, COM238, COM239, COM240,
   COM241, COM242, COM243, COM244, COM245, COM246, COM247, COM248, COM249, COM250,
   COM251, COM252, COM253, COM254, COM255, COM256, 
   LPT2,
   LPT3,
   NUM_OF_PORT_TYPES
};

#define COM_START COM1
#define COM_END COM256

// Get string representation of port type
CString GetEnumString(PortType nType);

/////////////////////////////////////////////////////////////////////////////
// PortCheck class
class PortCheck  
{
public:
   static bool IsSerialPort(CString oPortName);                               // Determine if the name passed is a serial (com) port
   static bool IsSerialPort(PortType nPort);                                  // Determine if the porttype is a serial (com) port
    
   static bool IsParallelPort(CString oPortName);                             // Determine if the name passed is a parallel (lpt) port
   static bool IsParallelPort(PortType nPort);                                // Determine if the porttype is a parallel (lpt) port

   static int GetPortAddress(CString oPortName);                              // Get physical address of port
   static int GetPortAddress(PortType nPort);                                 // Get physical address of port

   static void GetAvailablePorts(CStringArray &oPorts,bool bCOMonly = true, bool bLPTonly = false,void (*pSendStatusString)(CString) = NULL);  // Fill array with all available ports (serial only if bCOMonly = true)

   static PortStatus GetPortStatus(PortType nPort);                           // Get the current status of this port
	static PortStatus GetPortStatus(CString oPortName);                        // Get the current status of this port

   static CString FormatError();                                              // Get detailed message on status of the port last checked
   static UINT GetLastError() { return m_nErrorCode; }                        // Get the Microsoft error code returned on the last port chekced

private:
   static UINT m_nErrorCode;
};

#endif // !defined(ANADIGM_PORTCHECK_H_INCLUDED)
