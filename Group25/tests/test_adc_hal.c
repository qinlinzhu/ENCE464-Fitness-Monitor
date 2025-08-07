#include "unity.h"
#include "adc_hal.h"

#include "fff.h"
DEFINE_FFF_GLOBALS;
#define FFF_MOCK_IMPL // Includes mock implementations

#include "circBufT_mock.h"
#include "tiva_mocks/adc_mock.h"
#include "sysctl_mock.h"

#define ADC_BUF_SIZE 10
#define FAKE_ADC_VALUE 0xFACCEAD // No K's in hex

ADC_ID adc_id = ADC_ID_1; //Assuming ID we are given is 1
/* Helper functions */
void reset_fff(void)
{
    FFF_ADC_FAKES_LIST(RESET_FAKE);
    FFF_SYSCTL_FAKES_LIST(RESET_FAKE);
    FFF_CIRCBUFT_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}

void dummy_callback() {
    callback cb;
    cb = ADCIntHandler; 
}


circBuf_t* get_circBuf_ptr_and_reset_fff(void)
{
    adc_hal_register(adc_id, dummy_callback);
    circBuf_t* buffer_ptr = initCircBuf_fake.arg0_val;
    reset_fff();
    return buffer_ptr;
}

void assert_f1_called_before_f2(void* f1, void* f2)
{
    int8_t i_f1 = -1;
    int8_t i_f2 = -1;

    for (uint8_t i = 0; i < FFF_CALL_HISTORY_LEN; i++)
    {
        if(fff.call_history[i] == NULL)
            break;

        if (i_f1 == -1 && fff.call_history[i] == f1) i_f1 = i;
        if (i_f2 == -1 && fff.call_history[i] == f2) i_f2 = i;        
    }
    
    TEST_ASSERT(i_f1 < i_f2);
}

/* Custom fakes */
int32_t ADCSequenceDataGet_fake_adc_value(uint32_t arg0, uint32_t arg1,
                                  uint32_t *arg2)
{
    (void)arg0;
    (void)arg1;
    *arg2 = FAKE_ADC_VALUE;
    return 0;
}

bool SysCtlPeripheralReady_fake_bool_value(uint32_t arg0) 
{
    (void)arg0;
    return true;
}

/* Unity setup and teardown */
void setUp(void)
{
    reset_fff();
}

void tearDown(void)
{
    
}


/* Test cases - adc_hal_register() */
void test_adc_hal_register_initialises_buffer(void)
{   
    // Act
    adc_hal_register(adc_id, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, initCircBuf_fake.call_count);
    TEST_ASSERT_EQUAL(ADC_BUF_SIZE, initCircBuf_fake.arg1_val);
}

void test_adc_hal_register_enables_adc(void)
{
    // Act
    adc_hal_register(adc_id, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, SysCtlPeripheralEnable_fake.call_count);
    TEST_ASSERT_EQUAL(SYSCTL_PERIPH_ADC0, SysCtlPeripheralEnable_fake.arg0_val);
}

void test_adc_hal_register_initialises_adc_sequence(void)
{
    // Act
    adc_hal_register(adc_id, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, ADCSequenceConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceConfigure_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceConfigure_fake.arg1_val);  
    TEST_ASSERT_EQUAL(ADC_TRIGGER_PROCESSOR, ADCSequenceConfigure_fake.arg2_val);
    TEST_ASSERT_EQUAL(0, ADCSequenceConfigure_fake.arg3_val);
}

void test_adc_hal_register_wrong_id_doesnt_initialise_adc(void)
{
    //Act
    adc_hal_register(0, dummy_callback);

    // Assert
    TEST_ASSERT(ADC0_BASE != ADCSequenceStepConfigure_fake.arg0_val);

}

void test_adc_hal_register_initialises_adc_sequence_step(void)
{
    // Act
    adc_hal_register(adc_id, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, ADCSequenceStepConfigure_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceStepConfigure_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceStepConfigure_fake.arg1_val);  
    TEST_ASSERT_EQUAL(0, ADCSequenceStepConfigure_fake.arg2_val);
    TEST_ASSERT_EQUAL((ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END), ADCSequenceStepConfigure_fake.arg3_val);
}

void test_adc_hal_register_enables_adc_sequence(void)
{
    // Act
    adc_hal_register(adc_id, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, ADCSequenceEnable_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceEnable_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceEnable_fake.arg1_val);  
}


void test_adc_hal_register_enables_adc_before_other_adc_operations(void)
{
    // Act
    adc_hal_register(adc_id, dummy_callback);

    // Assert
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCSequenceConfigure);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCSequenceStepConfigure);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCSequenceEnable);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCIntRegister);
    assert_f1_called_before_f2((void*)SysCtlPeripheralEnable, (void*)ADCIntEnable);
}



/* Test cases - conversion function */
void test_adc_hal_poll_triggers_adc(void)
{
    // Act
    adc_hal_start_conversion(adc_id);
    // Assert
    TEST_ASSERT_EQUAL(1, ADCProcessorTrigger_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCProcessorTrigger_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCProcessorTrigger_fake.arg1_val);
    
}

void test_adc_hal_wrong_id_doesnt_trigger_adc_during_conversion(void)
{
    // Act
    adc_id = 0; // Unavailable ID
    adc_hal_start_conversion(0);
    // Assert
    TEST_ASSERT_EQUAL(0, ADCProcessorTrigger_fake.call_count);
    
    
}


/* Test cases - ADCIntHandler */
void test_adc_hal_register_adc_interrupt(void)
{
    // Act
    adc_hal_register(adc_id, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, ADCIntRegister_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCIntRegister_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCIntRegister_fake.arg1_val);  
    TEST_ASSERT_EQUAL(ADCIntHandler, ADCIntRegister_fake.arg2_val);
}

void test_adc_hal_register_enables_adc_interrupt(void)
{
    // Act
    adc_hal_register(adc_id, dummy_callback);
    
    // Assert
    TEST_ASSERT_EQUAL(1, ADCIntEnable_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCIntEnable_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCIntEnable_fake.arg1_val);  
}

void test_adc_hal_reads_from_correct_ADC_channel_and_sequence(void)
{
    ADCSequenceDataGet_fake.custom_fake = ADCSequenceDataGet_fake_adc_value;
    ADCIntHandler();

    TEST_ASSERT_EQUAL(ADC0_BASE, ADCSequenceDataGet_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCSequenceDataGet_fake.arg1_val);
}

void test_adc_hal_writes_to_correct_buffer(void)
{
    circBuf_t* expected_buffer_ptr = get_circBuf_ptr_and_reset_fff();
    ADCSequenceDataGet_fake.custom_fake = ADCSequenceDataGet_fake_adc_value;
    ADCIntHandler();

    TEST_ASSERT_EQUAL_PTR(expected_buffer_ptr, writeCircBuf_fake.arg0_val);
    TEST_ASSERT_EQUAL(FAKE_ADC_VALUE, writeCircBuf_fake.arg1_val);
}

void test_adc_hal_writes_to_correct_value_to_buffer(void)
{
    ADCSequenceDataGet_fake.custom_fake = ADCSequenceDataGet_fake_adc_value;
    ADCIntHandler();
    TEST_ASSERT_EQUAL(FAKE_ADC_VALUE, writeCircBuf_fake.arg1_val);
}

void test_adc_hal_clears_interrupt_signal(void)
{ 
    ADCSequenceDataGet_fake.custom_fake = ADCSequenceDataGet_fake_adc_value;
    ADCIntHandler();
    
    TEST_ASSERT_EQUAL(1, ADCIntClear_fake.call_count);
    TEST_ASSERT_EQUAL(ADC0_BASE, ADCIntClear_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, ADCIntClear_fake.arg1_val);
}

void test_SysCtlPeriph_is_ready(void) 
{
    // Act
    SysCtlPeripheralReady_fake.custom_fake = SysCtlPeripheralReady_fake_bool_value;
    adc_hal_register(adc_id, dummy_callback);

    // Assert
    TEST_ASSERT_EQUAL(1, SysCtlPeripheralReady_fake.call_count);
      
}


/* Test cases - readADC */
void test_adc_hal_read_correct_number_of_buffer_reads(void)
{
    // Arrange
    int32_t vals[ADC_BUF_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    SET_RETURN_SEQ(readCircBuf, vals, ADC_BUF_SIZE);
    // Act
    readADC();
    // Assert
    TEST_ASSERT_EQUAL(ADC_BUF_SIZE, readCircBuf_fake.call_count);
}

void test_adc_hal_read_correct_average_value(void)
{
    // Arrange
    uint32_t vals[ADC_BUF_SIZE] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    uint32_t average = 55; // Sum is 550, divided by 10 is 55
    SET_RETURN_SEQ(readCircBuf, vals, ADC_BUF_SIZE);

    // Act
    uint32_t result = readADC();

    // Assert
    TEST_ASSERT_EQUAL(average, result);
}

void test_adc_hal_read_with_rounding_error(void)
{
    // Arrange
    uint32_t vals[ADC_BUF_SIZE] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    uint32_t average = 1; 
    SET_RETURN_SEQ(readCircBuf, vals, ADC_BUF_SIZE);

    // Act 
    uint32_t result = readADC();

    // Assert
    TEST_ASSERT_EQUAL(average, result);
}