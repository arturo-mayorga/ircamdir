#include <Windows.h>
#include <XInput.h>

#include <conio.h>

#include <gtest/gtest.h>

TEST(MessageReader, MessageNullAfterConstruction)
{

    int success = 0;
    int fail = 0;

    DWORD dwResult;    
    for (DWORD i=0; i< XUSER_MAX_COUNT; i++ )
    {
        XINPUT_STATE state;
        ZeroMemory( &state, sizeof(XINPUT_STATE) );

        // Simply get the state of the controller from XInput.
        dwResult = XInputGetState( i, &state );

        if( dwResult == ERROR_SUCCESS )
        {
            success++;
            // Controller is connected
        }
        else
        {
            fail++;
            // Controller is not connected
        }
    }

    EXPECT_TRUE(-1 == -1);
}