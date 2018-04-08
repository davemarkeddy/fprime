// ====================================================================== 
// \title  AMPCS.cpp
// \author Rob Bocchino
// \brief  AMPCS-specific tests
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#include "Os/FileSystem.hpp"
#include "Svc/CmdSequencer/test/ut/AMPCS.hpp"

namespace Svc {

  namespace AMPCS {

    // ----------------------------------------------------------------------
    // Constructors 
    // ----------------------------------------------------------------------

    Tester ::
      Tester(void) :
        Svc::Tester(SequenceFiles::File::Format::AMPCS)
    {

    }

    // ----------------------------------------------------------------------
    // Tests 
    // ----------------------------------------------------------------------

    void Tester ::
      MissingCRC(void)
    {
      // Write the file
      SequenceFiles::MissingCRCFile file(this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName);
      this->clearAndDispatch();
      // Assert no response on seqDone
      ASSERT_from_seqDone_SIZE(0);
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      Fw::EightyCharString crcFileName(fileName);
      crcFileName += ".CRC32";
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileNotFound(0, crcFileName.toChar());
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_Errors(0, 1);
    }

    void Tester ::
      MissingFile(void) 
    {
      // Remove the file
      SequenceFiles::MissingFile file(this->format);
      const char *const fileName = file.getName().toChar();
      file.write();
      file.remove();
      // Run the sequence
      this->sendCmd_CS_RUN(0, 0, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_RUN,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileInvalid(
          0,
          fileName,
          CmdSequencerComponentBase::SEQ_READ_HEADER_SIZE,
          Os::FileSystem::INVALID_PATH
      );
      // Assert telemetry
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_CS_Errors(0, 1);
    }

  }

}
