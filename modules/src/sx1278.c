#include"sx1278.h"
#include<stdlib.h>
#include<stdio.h>

uint8_t SX1276Regs[0x71];
tSX1276LR * SX1276LR;
void *lgw_spi_target = NULL; /*! generic pointer to the SPI device */

// Default settings
tLoRaSettings LoRaSettings =
{
    434700000,        // RFFrequency
    20,               // Power
    9,                // SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                      // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
    12,                // SpreadingFactor [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    1,                // ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    1,                // CrcOn [0: OFF, 1: ON]
    0,                // ImplicitHeaderOn [0: OFF, 1: ON]
    0,                // RxSingleOn [0: Continuous, 1 Single]
    0,                // FreqHopOn [0: OFF, 1: ON]
    4,                // HopPeriod Hops every FREQUENCY_HOPPING_PERIOD symbols
    100,              // TxPacketTimeout
    100,              // RxPacketTimeout
    64,               // PayloadLength         // PayloadLength (used for implicit header mode)
};
const int32_t HoppingFrequencies[] =
{
    916500000,
    923500000,
    906500000,
    917500000,
    917500000,
    909000000,
    903000000,
    916000000,
    912500000,
    926000000,
    925000000,
    909500000,
    913000000,
    918500000,
    918500000,
    902500000,
    911500000,
    926500000,
    902500000,
    922000000,
    924000000,
    903500000,
    913000000,
    922000000,
    926000000,
    910000000,
    920000000,
    922500000,
    911000000,
    922000000,
    909500000,
    926000000,
    922000000,
    918000000,
    925500000,
    908000000,
    917500000,
    926500000,
    908500000,
    916000000,
    905500000,
    916000000,
    903000000,
    905000000,
    915000000,
    913000000,
    907000000,
    910000000,
    926500000,
    925500000,
    911000000,
};
const double RssiOffsetLF[] =
{   // These values need to be specify in the Lab
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
};

const double RssiOffsetHF[] =
{   // These values need to be specify in the Lab
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
};
const double SignalBwLog[] =
{
    3.8927900303521316335038277369285,  // 7.8 kHz
    4.0177301567005500940384239336392,  // 10.4 kHz
    4.193820026016112828717566631653,   // 15.6 kHz
    4.31875866931372901183597627752391, // 20.8 kHz
    4.4948500216800940239313055263775,  // 31.2 kHz
    4.6197891057238405255051280399961,  // 41.6 kHz
    4.795880017344075219145044421102,   // 62.5 kHz
    5.0969100130080564143587833158265,  // 125 kHz
    5.397940008672037609572522210551,   // 250 kHz
    5.6989700043360188047862611052755   // 500 kHz
};

int switch_tx_rx(int flag)
{   
    if(flag == 0) //接收模式
    {
        //printf("#############set to rx ###########\n");
        system("echo 1 > /sys/class/gpio/gpio23/value");
        system("echo 0 > /sys/class/gpio/gpio18/value");
    }
    else    //发送模式
    {
        //printf("#############set to tx ###########\n");
        system("echo 0 > /sys/class/gpio/gpio23/value");
        system("echo 1 > /sys/class/gpio/gpio18/value");        
    }
}

void SX1276LoRaSetOpMode(uint8_t opMode)
{
    static uint8_t opModePrev = RFLR_OPMODE_STANDBY;
    static bool antennaSwitchTxOnPrev = true;
    bool antennaSwitchTxOn = false;

    opModePrev = SX1276LR->RegOpMode & ~RFLR_OPMODE_MASK;
    printf("SX1276LoRaSetOpMode SX1276LR->RegOpMode:0x%0x opMode:0x%0x opModePrev:0x%0x\n", SX1276LR->RegOpMode, opMode, opModePrev);
    if( opMode != opModePrev )
    {
        if(opMode == RFLR_OPMODE_TRANSMITTER)
        {
            antennaSwitchTxOn = true;
        }
        else
        {
            antennaSwitchTxOn = false;
        }
        if( antennaSwitchTxOn != antennaSwitchTxOnPrev )
        {
            antennaSwitchTxOnPrev = antennaSwitchTxOn;
            switch_tx_rx( antennaSwitchTxOn ); // Antenna switch control
        }
        SX1276LR->RegOpMode = ( SX1276LR->RegOpMode & RFLR_OPMODE_MASK ) | opMode;
        printf("SX1276LoRaSetOpMode SX1276LR->RegOpMode:0x%0x end\n",SX1276LR->RegOpMode );
        spi_write(*(int * )lgw_spi_target,REG_LR_OPMODE,SX1276LR->RegOpMode);
        //SX1276Write( REG_LR_OPMODE, SX1276LR.RegOpMode );        
    }
}

void SX1276LoRaSetRFFrequency( uint32_t freq )
{
    char value;

    printf("freq:%d\n", freq);
    LoRaSettings.RFFrequency = freq;

    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );

    printf("freq:%d\n", freq);
    
    SX1276LR->RegFrfMsb = ( uint8_t )( ( freq >> 16 ) & 0xFF );
    SX1276LR->RegFrfMid = ( uint8_t )( ( freq >> 8 ) & 0xFF );
    SX1276LR->RegFrfLsb = ( uint8_t )( freq & 0xFF );


    printf("SX1276LR->RegFrfMsb:0x%0x SX1276LR->RegFrfMid:0x%0x SX1276LR->RegFrfLsb:0x%0x\n", 
        SX1276LR->RegFrfMsb, SX1276LR->RegFrfMid, SX1276LR->RegFrfLsb);
    
    // SX1276WriteBuffer( REG_LR_FRFMSB, &SX1276LR->RegFrfMsb, 3 );
    spi_write_buffer(*(int * )lgw_spi_target,REG_LR_FRFMSB,&SX1276LR->RegFrfMsb, 3);

    value = spi_read(*(int *)lgw_spi_target, REG_LR_FRFMSB);
    printf("REG_LR_FRFMSB:0x%0x SX1276LR->RegFrfMsb:0x%0x value:0x%0x\n", REG_LR_FRFMSB, SX1276LR->RegFrfMsb, value);

    value = spi_read(*(int *)lgw_spi_target, REG_LR_FRFMID);
    printf("REG_LR_FRFMID:0x%0x SX1276LR->RegFrfMid:0x%0x value:0x%0x\n", REG_LR_FRFMID, SX1276LR->RegFrfMid, value);

    value = spi_read(*(int *)lgw_spi_target, REG_LR_FRFLSB);
    printf("REG_LR_FRFLSB:0x%0x SX1276LR->RegFrfLsb:0x%0x value:0x%0x\n", REG_LR_FRFLSB, SX1276LR->RegFrfLsb, value);
}

void SX1276LoRaSetNbTrigPeaks( uint8_t value )
{
    SX1276LR->RegTestReserved31 = spi_read(*(int * )lgw_spi_target, 0x31);
    SX1276LR->RegTestReserved31 = ( SX1276LR->RegTestReserved31 & 0xF8 ) | value;
    spi_write(*(int * )lgw_spi_target,0x31, SX1276LR->RegTestReserved31);

    int tmpvalue;
    tmpvalue = spi_read(*(int *)lgw_spi_target,0x31);
    printf("SX1276LoRaSetNbTrigPeaks:0x%0x value:0x%0x\n", 0x31, tmpvalue);

}

void SX1276LoRaSetSpreadingFactor( uint8_t factor )
{
    if( factor > 12 )
    {
        factor = 12;
    }
    else if( factor < 6 )
    {
        factor = 6;
    }

    if( factor == 6 )
    {
        SX1276LoRaSetNbTrigPeaks( 5 );
    }
    else
    {
        SX1276LoRaSetNbTrigPeaks( 3 );
    }

    SX1276LR->RegModemConfig2 = spi_read(*(int * )lgw_spi_target, REG_LR_MODEMCONFIG2);    
    //printf("SX1276LR->RegModemConfig2:0x%0x line:%d\n", SX1276LR->RegModemConfig2, __LINE__);
   
    SX1276LR->RegModemConfig2 = ( SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_SF_MASK ) | ( factor << 4 );
    spi_write(*(int * )lgw_spi_target,REG_LR_MODEMCONFIG2, SX1276LR->RegModemConfig2);
    LoRaSettings.SpreadingFactor = factor;
}

void SX1276LoRaSetErrorCoding( uint8_t value )
{

    SX1276LR->RegModemConfig1 = spi_read(*(int * )lgw_spi_target, REG_LR_MODEMCONFIG1);
    SX1276LR->RegModemConfig1 = ( SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | ( value << 1 );
    spi_write(*(int * )lgw_spi_target,REG_LR_MODEMCONFIG1, SX1276LR->RegModemConfig1);

    int tmpValue;
    tmpValue = spi_read(*(int *)lgw_spi_target, REG_LR_MODEMCONFIG1);
    printf("REG_LR_MODEMCONFIG1:0x%0x RegModemConfig1:0x%0x tmpValue:0x%0x line:%d\n", REG_LR_MODEMCONFIG1, SX1276LR->RegModemConfig1, tmpValue, __LINE__);

    LoRaSettings.ErrorCoding = value;
}

void SX1276LoRaSetPacketCrcOn( bool enable )
{
    SX1276LR->RegModemConfig2 = spi_read(*(int * )lgw_spi_target, REG_LR_MODEMCONFIG2);
    printf("REG_LR_MODEMCONFIG2:0x%0x RegModemConfig2:0x%0x line:%d\n", REG_LR_MODEMCONFIG2, SX1276LR->RegModemConfig2, __LINE__);   

    SX1276LR->RegModemConfig2 = ( SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) | ( enable << 2 );
    spi_write(*(int * )lgw_spi_target,REG_LR_MODEMCONFIG2, SX1276LR->RegModemConfig2);
    LoRaSettings.CrcOn = enable;

    int tmpValue;
    tmpValue = spi_read(*(int *)lgw_spi_target, REG_LR_MODEMCONFIG2);
    printf("REG_LR_MODEMCONFIG2:0x%0x RegModemConfig2:0x%0x tmpValue:0x%0x line:%d\n", REG_LR_MODEMCONFIG2, SX1276LR->RegModemConfig2, tmpValue, __LINE__);

}

void SX1276LoRaSetSignalBandwidth( uint8_t bw )
{
    SX1276LR->RegModemConfig1 = spi_read(*(int * )lgw_spi_target, REG_LR_MODEMCONFIG1);
    
    SX1276LR->RegModemConfig1 = ( SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_BW_MASK ) | ( bw << 4 );
    spi_write(*(int * )lgw_spi_target,REG_LR_MODEMCONFIG1, SX1276LR->RegModemConfig1);
    LoRaSettings.SignalBw = bw;
}

void SX1276LoRaSetImplicitHeaderOn( bool enable )
{
    SX1276LR->RegModemConfig1 = spi_read(*(int * )lgw_spi_target, REG_LR_MODEMCONFIG1);
    SX1276LR->RegModemConfig1 = ( SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable );
    spi_write(*(int * )lgw_spi_target,REG_LR_MODEMCONFIG1, SX1276LR->RegModemConfig1);
    LoRaSettings.ImplicitHeaderOn = enable;
}

void SX1276LoRaSetSymbTimeout( uint16_t value )
{
    SX1276LR->RegModemConfig2 = spi_read(*(int * )lgw_spi_target, REG_LR_MODEMCONFIG2);
    SX1276LR->RegSymbTimeoutLsb = spi_read(*(int * )lgw_spi_target, REG_LR_SYMBTIMEOUTLSB);

    printf("REG_LR_MODEMCONFIG2:0x%0x RegModemConfig2:0x%0x line:%d\n", REG_LR_MODEMCONFIG2, SX1276LR->RegModemConfig2, __LINE__);   


    SX1276LR->RegModemConfig2 = ( SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) | ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK );
    SX1276LR->RegSymbTimeoutLsb = value & 0xFF;
    spi_write_buffer(*(int * )lgw_spi_target,REG_LR_MODEMCONFIG2, &SX1276LR->RegModemConfig2,2);

    int tmpValue;
    tmpValue = spi_read(*(int *)lgw_spi_target, REG_LR_MODEMCONFIG2);
    printf("REG_LR_MODEMCONFIG2:0x%0x RegModemConfig2:0x%0x tmpValue:0x%0x line:%d\n", REG_LR_MODEMCONFIG2, SX1276LR->RegModemConfig2, tmpValue, __LINE__);   

    tmpValue = spi_read(*(int *)lgw_spi_target, REG_LR_SYMBTIMEOUTLSB);
    printf("REG_LR_SYMBTIMEOUTLSB:0x%0x RegSymbTimeoutLsb:0x%0x tmpValue:0x%0x line:%d\n", REG_LR_SYMBTIMEOUTLSB, SX1276LR->RegSymbTimeoutLsb, tmpValue, __LINE__);   

}

void SX1276LoRaSetPayloadLength( uint8_t value )
{
    SX1276LR->RegPayloadLength = value;
    spi_write(*(int * )lgw_spi_target,REG_LR_PAYLOADLENGTH, SX1276LR->RegPayloadLength);
   
    LoRaSettings.PayloadLength = value;
}
void SX1276LoRaSetLowDatarateOptimize( bool enable )
{
    
    SX1276LR->RegModemConfig3 = spi_read(*(int * )lgw_spi_target, REG_LR_MODEMCONFIG3);
    SX1276LR->RegModemConfig3 = ( SX1276LR->RegModemConfig3 & RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) | ( enable << 3 );
    spi_write(*(int * )lgw_spi_target,REG_LR_MODEMCONFIG3, SX1276LR->RegModemConfig3);
}
void SX1276LoRaSetPAOutput( uint8_t outputPin )
{
    
    SX1276LR->RegPaConfig = spi_read(*(int * )lgw_spi_target, REG_LR_PACONFIG);
    SX1276LR->RegPaConfig = (SX1276LR->RegPaConfig & RFLR_PACONFIG_PASELECT_MASK ) | outputPin;
    spi_write(*(int * )lgw_spi_target,REG_LR_PACONFIG, SX1276LR->RegPaConfig);
}
void SX1276LoRaSetPa20dBm( bool enale )
{
    SX1276LR->RegPaDac = spi_read(*(int * )lgw_spi_target, REG_LR_PADAC);
    SX1276LR->RegPaConfig = spi_read(*(int * )lgw_spi_target, REG_LR_PACONFIG);
    

    if( ( SX1276LR->RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
    {    
        if( enale == true )
        {
            SX1276LR->RegPaDac = 0x87;
        }
    }
    else
    {
        SX1276LR->RegPaDac = 0x84;
    }
    spi_write(*(int * )lgw_spi_target,REG_LR_PADAC, SX1276LR->RegPaDac);

    int tmpValue;
    tmpValue = spi_read(*(int *)lgw_spi_target, REG_LR_PADAC);
    printf("REG_LR_PADAC:0x%0x RegPaDac:0x%0x tmpValue:0x%0x\n", REG_LR_PADAC, SX1276LR->RegPaDac, tmpValue);   
}

void SX1276LoRaSetRFPower( int8_t power )
{


    SX1276LR->RegPaConfig = spi_read(*(int * )lgw_spi_target, REG_LR_PACONFIG);
    SX1276LR->RegPaDac = spi_read(*(int * )lgw_spi_target, REG_LR_PADAC);

    if( ( SX1276LR->RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
    {
        if( ( SX1276LR->RegPaDac & 0x87 ) == 0x87 )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            SX1276LR->RegPaConfig = ( SX1276LR->RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
            SX1276LR->RegPaConfig = ( SX1276LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if( power < 2 )
            {
                power = 2;
            }
            if( power > 17 )
            {
                power = 17;
            }
            SX1276LR->RegPaConfig = ( SX1276LR->RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
            SX1276LR->RegPaConfig = ( SX1276LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }
    else
    {
        if( power < -1 )
        {
            power = -1;
        }
        if( power > 14 )
        {
            power = 14;
        }
        SX1276LR->RegPaConfig = ( SX1276LR->RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
        SX1276LR->RegPaConfig = ( SX1276LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    
    spi_write(*(int * )lgw_spi_target,REG_LR_PACONFIG, SX1276LR->RegPaConfig);
    
    LoRaSettings.Power = power;
}


void SX1276LoRaSetFIFO( uint32_t value )
{
    int ret;

    ret = spi_read(*(int * )lgw_spi_target, REG_LR_FIFO);
    printf("REG_LR_FIFO:0x%0x\n", ret);
    
    spi_write(*(int * )lgw_spi_target,REG_LR_FIFO, value);

    SX1276LR->RegFifo = value;
    ret = spi_read(*(int * )lgw_spi_target, REG_LR_FIFO);
    printf("REG_LR_FIFO:0x%0x\n", ret);
}

void SX1276Init()
{
    SX1276LR = (tSX1276LR *)SX1276Regs;
    int spi_stat = SUCCESS;
    spi_stat = lgw_spi_open(&lgw_spi_target);/* open the SPI link */
    if (spi_stat != SUCCESS)
    {
        printf("ERROR CONNECTING CONCENTRATOR\n");
        return ;
    }
    
    unsigned char value = spi_read(*(int *)lgw_spi_target, 0x42);
    
    while(value != 0x12)
    {
        printf("Hard SPI Err!\r\n");
        return ;
    }


    SX1276LoRaSetOpMode(RFLR_OPMODE_SLEEP);
    SX1276LR->RegOpMode = ( SX1276LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON;
    spi_write(*(int * )lgw_spi_target,REG_LR_OPMODE,SX1276LR->RegOpMode);
    SX1276LoRaSetOpMode(RFLR_OPMODE_STANDBY);

    SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
    SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
    spi_write_buffer(*(int * )lgw_spi_target,REG_LR_DIOMAPPING1,&SX1276LR->RegDioMapping1,2);

    //读取寄存器的值到SX1276Regs
    int j;
    for(j = 1; j <= 0x70; j++)
    {
        SX1276Regs[j] = spi_read(*(int *)lgw_spi_target, j);
    }
    
        
    SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );
    SX1276LoRaSetRFFrequency( LoRaSettings.RFFrequency);    
    SX1276LoRaSetSpreadingFactor( LoRaSettings.SpreadingFactor );//config2 
    SX1276LoRaSetErrorCoding( LoRaSettings.ErrorCoding );
    SX1276LoRaSetPacketCrcOn( LoRaSettings.CrcOn );//config2
    SX1276LoRaSetSignalBandwidth( LoRaSettings.SignalBw );
    SX1276LoRaSetImplicitHeaderOn( LoRaSettings.ImplicitHeaderOn );
    SX1276LoRaSetSymbTimeout( 0x3FF );//config2
    SX1276LoRaSetPayloadLength( LoRaSettings.PayloadLength );
    SX1276LoRaSetLowDatarateOptimize( true );

    if( LoRaSettings.RFFrequency > 360000000 )  //860000000
    {
        SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_PABOOST ); //选择 PA_BOOST 管脚输出信号
        SX1276LoRaSetPa20dBm( true );  //最大输出功率 有问题 寄存器不一样
        LoRaSettings.Power = 20;
        SX1276LoRaSetRFPower(LoRaSettings.Power);
    }
    else
    {
        SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_RFO );
        SX1276LoRaSetPa20dBm( false );
        LoRaSettings.Power = 14;
        SX1276LoRaSetRFPower( LoRaSettings.Power );
    } 

    SX1276LoRaSetOpMode(RFLR_OPMODE_STANDBY);
}