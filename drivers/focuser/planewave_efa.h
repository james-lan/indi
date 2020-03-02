/*
    PlaneWave EFA Protocol

    Hendrick Focuser

    Copyright (C) 2020 Jasem Mutlaq (mutlaqja@ikarustech.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#pragma once

#include "indifocuser.h"

#include <map>

class EFA : public INDI::Focuser
{
    public:
        EFA();

        virtual bool Handshake();
        const char *getDefaultName();
        virtual bool initProperties();
        virtual bool updateProperties();

        virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n);
        virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n);

        enum
        {
            MTR_GET_POS = 0x01,
            MTR_GOTO_POS2 = 0x17,
            MTR_OFFSET_CNT = 0x04,
            MTR_GOTO_OVER = 0x13,
            MTR_SLEWLIMITMAX = 0x1B,
            MTR_SLEWLIMITGETMAX = 0x1D,
            MTR_PMSLEW_RATE = 0x24,
            MTR_NMSLEW_RATE = 0x25,
            TEMP_GET = 0x26,
            FANS_SET = 0x27,
            FANS_GET = 0x28,
            MTR_GET_CALIBRATION_STATE = 0x30,
            MTR_SET_CALIBRATION_STATE = 0x31,
            MTR_GET_STOP_DETECT = 0xEE,
            MTR_STOP_DETECT = 0xEF,
            MTR_GET_APPROACH_DIRECTION = 0xFC,
            MTR_APPROACH_DIRECTION = 0xFD,
            GET_VERSION = 0xFE
        };

        enum
        {
            DEVICE_PC = 0x20,
            DEVICE_HC = 0x0D,
            DEVICE_FOC = 0x12,
            DEVICE_FAN = 0x13,
            DEVICE_TEMP = 0x12
        };

    protected:
        virtual IPState MoveAbsFocuser(uint32_t targetTicks);
        virtual IPState MoveRelFocuser(FocusDirection dir, unsigned int ticks);
        virtual bool SyncFocuser(uint32_t ticks);
        virtual bool ReverseFocuser(bool enabled);
        virtual bool SetFocuserMaxPosition(uint32_t ticks);
        virtual bool AbortFocuser();
        virtual void TimerHit();

        virtual bool saveConfigItems(FILE *fp);

    private:
        ///////////////////////////////////////////////////////////////////////////////////
        /// Query functions
        ///////////////////////////////////////////////////////////////////////////////////
        bool isGOTOComplete();
        bool readPosition();
        bool readTemperature();
        bool readFanState();
        bool readCalibrationState();

        ///////////////////////////////////////////////////////////////////////////////////
        /// Set functions
        ///////////////////////////////////////////////////////////////////////////////////
        bool setFanEnabled(bool enabled);
        bool setCalibrationEnabled(bool enabled);

        ///////////////////////////////////////////////////////////////////////////////
        /// Communication Functions
        ///////////////////////////////////////////////////////////////////////////////
        bool sendCommandOk(const char * cmd, int cmd_len);
        bool sendCommand(const char * cmd, char * res, int cmd_len, int res_len);
        void hexDump(char * buf, const char * data, int size);
        std::vector<std::string> split(const std::string &input, const std::string &regex);

        ///////////////////////////////////////////////////////////////////////////////////
        /// Misc
        ///////////////////////////////////////////////////////////////////////////////////
        void getStartupValues();
        double calculateTemperature(uint8_t byte2, uint8_t byte3);
        uint8_t calculateCheckSum(const char *cmd, uint32_t len);
        template <typename T> std::string to_string(const T a_value, const int n = 2);

        ///////////////////////////////////////////////////////////////////////////////////
        /// Properties
        ///////////////////////////////////////////////////////////////////////////////////

        // Focuser Informatin
        ITextVectorProperty InfoTP;
        IText InfoT[1];
        enum
        {
            INFO_VERSION
        };


        // FAN State
        ISwitchVectorProperty FanStateSP;
        ISwitch FanStateS[2];
        enum
        {
            FAN_ON,
            FAN_OFF
        };

        // Calibration State
        ISwitchVectorProperty CalibrationStateSP;
        ISwitch CalibrationStateS[2];
        enum
        {
            CALIBRATION_ON,
            CALIBRATION_OFF
        };

        // Read Only Temperature Reporting
        INumberVectorProperty TemperatureNP;
        INumber TemperatureN[3];
        enum
        {
            TEMPERATURE_PRIMARY,
            TEMPERATURE_AMBIENT,
            TEMPERATURE_SECONDARY
        };

        /////////////////////////////////////////////////////////////////////////////
        /// Private variables
        /////////////////////////////////////////////////////////////////////////////
        double m_LastTemperature {0};
        double m_LastPosition {0};

        /////////////////////////////////////////////////////////////////////////////
        /// Static Helper Values
        /////////////////////////////////////////////////////////////////////////////
        // Start of Message
        static const char DRIVER_SOM { 0x3B };
        // Temperature Reporting threshold
        static constexpr double TEMPERATURE_THRESHOLD { 0.05 };

        static constexpr const uint8_t DRIVER_LEN {9};
        // Wait up to a maximum of 3 seconds for serial input
        static constexpr const uint8_t DRIVER_TIMEOUT {3};
};
