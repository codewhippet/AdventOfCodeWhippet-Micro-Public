#include "Hardware.h"
#include "Hardware.h"
#include "Hardware.h"
#include "stdafx.h"
#include <assert.h>

#ifdef PICO_ON_DEVICE
#include <pico/stdlib.h>
#include <pico/divider.h>
#ifdef CYW43_WL_GPIO_LED_PIN
#include <pico/cyw43_arch.h>
#endif
#if PICO_RP2350
#include <hardware/structs/ioqspi.h>
#include <hardware/structs/qmi.h>
#include <hardware/structs/xip_ctrl.h>
#include <hardware/clocks.h>
#include <hardware/sync.h>
#endif
#endif

// ----------------------------------------------------------------------------

#if _WIN32

static std::vector<uint8_t> Psram;

void Hardware::Initialise()
{
	Psram.resize(8 * 1024 * 1024);
}

void Hardware::SetLedOn(bool)
{
}

void Hardware::FlashLed(int8_t)
{
}

void Hardware::FlashingStop(int8_t)
{
	__debugbreak();
}

int32_t Hardware::DivModRem(int32_t a, int32_t b, int32_t* rem)
{
	*rem = a % b;
	return a / b;
}

void* Hardware::PsramBase()
{
	return Psram.data();
}

size_t Hardware::PsramSize()
{
	return Psram.size();
}

#endif

// ----------------------------------------------------------------------------

#ifdef PICO_ON_DEVICE

void __not_in_flash_func(PsramInitialise)();

static void* PsramBasePtr = nullptr;
static size_t PsramSizeBytes = 0;

void Hardware::Initialise()
{
#if PICO_RP2350
    //set_sys_clock_khz(300000, true);
#endif

#if defined(PICO_DEFAULT_LED_PIN)
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#elif defined(CYW43_WL_GPIO_LED_PIN)
	cyw43_arch_init();
#endif

	PsramInitialise();
}

void Hardware::SetLedOn(bool state)
{
#if defined(PICO_DEFAULT_LED_PIN)
	gpio_put(PICO_DEFAULT_LED_PIN, state);
#elif defined(CYW43_WL_GPIO_LED_PIN)
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, state);
#endif
}

void Hardware::FlashLed(int8_t count)
{
	for (int i = 0; i < count; i++)
	{
		SetLedOn(true);
		sleep_ms(100);
		SetLedOn(false);
		sleep_ms(100);

		SetLedOn(true);
		sleep_ms(100);
		SetLedOn(false);
		sleep_ms(100);

		sleep_ms(250);
	}
}

void Hardware::FlashingStop(int8_t count)
{
	while (true)
	{
		for (int i = 0; i < count; i++)
		{
			SetLedOn(true);
			sleep_ms(100);
			SetLedOn(false);
			sleep_ms(100);
		}

		sleep_ms(500);
	}
}

int32_t Hardware::DivModRem(int32_t a, int32_t b, int32_t* rem)
{
    return divmod_s32s32_rem(a, b, rem);
}

#if PICO_RP2350
inline void PsramEnterDirectMode(uint32_t clkDivisor)  __attribute__((always_inline));
inline void PsramEnterDirectMode(uint32_t clkDivisor)
{
    uint32_t enterDirectMode = 0;
    enterDirectMode |= clkDivisor << QMI_DIRECT_CSR_CLKDIV_LSB;
    enterDirectMode |= QMI_DIRECT_CSR_EN_BITS;
    qmi_hw->direct_csr = enterDirectMode;
    while (qmi_hw->direct_csr & QMI_DIRECT_CSR_BUSY_BITS)
        tight_loop_contents();
}

inline uint8_t PsramWriteAndReadDirectSPI(uint8_t write) __attribute__((always_inline));
inline uint8_t PsramWriteAndReadDirectSPI(uint8_t write)
{
    qmi_hw->direct_tx = write;
    while (qmi_hw->direct_csr & QMI_DIRECT_CSR_BUSY_BITS)
        tight_loop_contents();
    return qmi_hw->direct_rx;
}

inline void PsramExitDirectMode()  __attribute__((always_inline));
inline void PsramExitDirectMode()
{
    qmi_hw->direct_csr &= ~QMI_DIRECT_CSR_EN_BITS;
    while (qmi_hw->direct_csr & QMI_DIRECT_CSR_BUSY_BITS)
        tight_loop_contents();
}
#endif

void __not_in_flash_func(PsramInitialise)()
{
#if PICO_RP2350 && defined PIMORONI_PICO_PLUS2_PSRAM_CS_PIN
    gpio_set_function(PIMORONI_PICO_PLUS2_PSRAM_CS_PIN, GPIO_FUNC_XIP_CS1);

    const uint32_t systemClockHz = clock_get_hz(clk_sys);

    const uint32_t MHz = 1000 * 1000;

    const uint32_t maxDirectModeHz = 33 * MHz; // 33MHz max for Read ID command
    const uint32_t directModeDivisor = (systemClockHz + maxDirectModeHz - 1) / maxDirectModeHz;

    const uint32_t maxQuadModeHz = 133 * MHz; // 133MHz maximum PSRAM frequency
    uint32_t quadModeDivisor = (systemClockHz + maxQuadModeHz - 1) / maxQuadModeHz;

    // Timing calculation and control codes cribbed from:
    // https://github.com/micropython/micropython/blob/master/ports/rp2/rp2_psram.c
    // https://eu.mouser.com/datasheet/3/4815/1/APM_PSRAM_E3_QSPI_APS6404L_3SQR_KGD_PKG_v2.7.pdf

    // If we're running the system clock above 100MHz, we must use a divisor > 1
    if (systemClockHz > 100 * MHz)
    {
        quadModeDivisor = std::min(quadModeDivisor, 2ul);
    }

    // If we're running the PSRAM above 100MHz, we need to add in one extra rxdelay
    uint32_t quadModeRxdelay = quadModeDivisor;
    if ((systemClockHz / quadModeDivisor) > 100 * MHz)
    {
        quadModeRxdelay++;
    }

    // Maximum select: 8us
    // Minimum deselect: 18ns
    const uint32_t maxSelect = systemClockHz / (8000000); // (8us / cycles per us) / 64 := (8 * Hz / 1,000,000) / 64 := (Hz / 125,000) / 64 := Hz / 8,000,000
    const uint32_t systemClockMHz = (systemClockHz + MHz - 1) / MHz;
    const uint32_t implicitDeselect = (quadModeDivisor + 1) / 2;
    const uint32_t extraDeselect = ((18 * systemClockMHz + 999) / 1000) - implicitDeselect;

    // Disable interrupts
    uint32_t interupts = save_and_disable_interrupts();

    // Enter direct mode with safe defaults
    PsramEnterDirectMode(directModeDivisor);

    // Make sure we're in SPI mode by sending a quad mode exit command
    {
        qmi_hw->direct_csr |= QMI_DIRECT_CSR_ASSERT_CS1N_BITS;
        qmi_hw->direct_tx = 0xf5 // Exit quad mode
            | QMI_DIRECT_TX_OE_BITS // Output
            | QMI_DIRECT_TX_IWIDTH_VALUE_Q << QMI_DIRECT_TX_IWIDTH_LSB // Sent in quad mode
            ;
        while (qmi_hw->direct_csr & QMI_DIRECT_CSR_BUSY_BITS)
            tight_loop_contents();

        (void)qmi_hw->direct_rx;
        qmi_hw->direct_csr &= ~QMI_DIRECT_CSR_ASSERT_CS1N_BITS;
    }

    // Read the ID to get the density
    uint8_t kgd = 0;
    uint8_t eid = 0;
    {
        qmi_hw->direct_csr |= QMI_DIRECT_CSR_ASSERT_CS1N_BITS;
        PsramWriteAndReadDirectSPI(0x9f); // Read ID
        PsramWriteAndReadDirectSPI(0xff); // Dummy address
        PsramWriteAndReadDirectSPI(0xff); // Dummy address
        PsramWriteAndReadDirectSPI(0xff); // Dummy address
        PsramWriteAndReadDirectSPI(0xff); // MF ID
        kgd = PsramWriteAndReadDirectSPI(0xff);
        eid = PsramWriteAndReadDirectSPI(0xff);
        qmi_hw->direct_csr &= ~QMI_DIRECT_CSR_ASSERT_CS1N_BITS;
    }

    if (kgd != 0x5d)
    {
        // No valid PSRAM chip
        PsramExitDirectMode();
        restore_interrupts(interupts);
        return;
    }

    // We have PSRAM!

    // Put the PSRAM into Quad mode
    {
        qmi_hw->direct_csr |= QMI_DIRECT_CSR_ASSERT_CS1N_BITS;
        PsramWriteAndReadDirectSPI(0x35);
        qmi_hw->direct_csr &= ~QMI_DIRECT_CSR_ASSERT_CS1N_BITS;
    }

    // Set up the PSRAM timing
    qmi_hw->m[1].timing =
        1 << QMI_M1_TIMING_COOLDOWN_LSB |
        QMI_M1_TIMING_PAGEBREAK_VALUE_1024 << QMI_M1_TIMING_PAGEBREAK_LSB |
        maxSelect << QMI_M1_TIMING_MAX_SELECT_LSB |
        extraDeselect << QMI_M1_TIMING_MIN_DESELECT_LSB |
        quadModeRxdelay << QMI_M1_TIMING_RXDELAY_LSB |
        quadModeDivisor << QMI_M1_TIMING_CLKDIV_LSB;

    // Tell QMI how to interface with the PSRAM
    qmi_hw->m[1].rcmd = 0xeb;
    qmi_hw->m[1].rfmt =
        QMI_M0_RFMT_DUMMY_LEN_VALUE_24 << QMI_M0_RFMT_DUMMY_LEN_LSB |
        QMI_M0_RFMT_PREFIX_LEN_VALUE_8 << QMI_M0_RFMT_PREFIX_LEN_LSB |
        QMI_M0_RFMT_DATA_WIDTH_VALUE_Q << QMI_M0_RFMT_DATA_WIDTH_LSB |
        QMI_M0_RFMT_DUMMY_WIDTH_VALUE_Q << QMI_M0_RFMT_DUMMY_WIDTH_LSB |
        QMI_M0_RFMT_SUFFIX_WIDTH_VALUE_Q << QMI_M0_RFMT_SUFFIX_WIDTH_LSB |
        QMI_M0_RFMT_ADDR_WIDTH_VALUE_Q << QMI_M0_RFMT_ADDR_WIDTH_LSB |
        QMI_M0_RFMT_PREFIX_WIDTH_VALUE_Q << QMI_M0_RFMT_PREFIX_WIDTH_LSB;

    qmi_hw->m[1].wcmd = 0x38;
    qmi_hw->m[1].wfmt =
        QMI_M0_WFMT_PREFIX_LEN_VALUE_8 << QMI_M0_WFMT_PREFIX_LEN_LSB |
        QMI_M0_WFMT_DATA_WIDTH_VALUE_Q << QMI_M0_WFMT_DATA_WIDTH_LSB |
        QMI_M0_WFMT_DUMMY_WIDTH_VALUE_Q << QMI_M0_WFMT_DUMMY_WIDTH_LSB |
        QMI_M0_WFMT_SUFFIX_WIDTH_VALUE_Q << QMI_M0_WFMT_SUFFIX_WIDTH_LSB |
        QMI_M0_WFMT_ADDR_WIDTH_VALUE_Q << QMI_M0_WFMT_ADDR_WIDTH_LSB |
        QMI_M0_WFMT_PREFIX_WIDTH_VALUE_Q << QMI_M0_WFMT_PREFIX_WIDTH_LSB;

    // Put the system back into a working state with writeable PSRAM
    PsramExitDirectMode();
    hw_set_bits(&xip_ctrl_hw->ctrl, XIP_CTRL_WRITABLE_M1_BITS);
    restore_interrupts(interupts);

    PsramSizeBytes = 2 * 1024 * 1024 << (eid >> 5);
    PsramBasePtr = reinterpret_cast<void*>(0x11000000);
#endif
}

void* Hardware::PsramBase()
{
	return PsramBasePtr;
}

size_t Hardware::PsramSize()
{
	return PsramSizeBytes;
}

#endif
