#include "bee.fish.h"

#include "camera_pins.h"
#include "esp_log.h"
#include "esp_camera.h"

using namespace BeeFishParser;

class Camera
{
private:
    bool _initialized = false;

    camera_fb_t *fb = nullptr;
    size_t _frameBufferCount = 0;
    framesize_t _frameSize = FRAMESIZE_XGA;

public:
    Camera()
    {
    }

    esp_err_t initialize(size_t frameBufferCount = 1)
    {

        //frameBufferCount = 1;
        if (fb)
        {
            esp_camera_fb_return(fb);
            fb = nullptr;
        }

        if (_initialized &&
            frameBufferCount == _frameBufferCount)
            return ESP_OK;

        if (!_initialized || frameBufferCount != _frameBufferCount)
        {

            if (_initialized)
                esp_camera_deinit();

            camera_config_t config;
            config.ledc_channel = LEDC_CHANNEL_0;
            config.ledc_timer = LEDC_TIMER_0;
            config.pin_d0 = Y2_GPIO_NUM;
            config.pin_d1 = Y3_GPIO_NUM;
            config.pin_d2 = Y4_GPIO_NUM;
            config.pin_d3 = Y5_GPIO_NUM;
            config.pin_d4 = Y6_GPIO_NUM;
            config.pin_d5 = Y7_GPIO_NUM;
            config.pin_d6 = Y8_GPIO_NUM;
            config.pin_d7 = Y9_GPIO_NUM;
            config.pin_xclk = XCLK_GPIO_NUM;
            config.pin_pclk = PCLK_GPIO_NUM;
            config.pin_vsync = VSYNC_GPIO_NUM;
            config.pin_href = HREF_GPIO_NUM;
            config.pin_sscb_sda = SIOD_GPIO_NUM;
            config.pin_sscb_scl = SIOC_GPIO_NUM;
            config.pin_pwdn = PWDN_GPIO_NUM;
            config.pin_reset = RESET_GPIO_NUM;
            config.xclk_freq_hz = 20000000;
            config.pixel_format = PIXFORMAT_JPEG;
            config.frame_size = FRAMESIZE_UXGA;
            config.jpeg_quality = 0;
            config.fb_count = frameBufferCount;

            // camera init
            esp_err_t err = esp_camera_init(&config);

            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Camera Init Failed 0x%x", err);
                return err;
            }

            _initialized = true;
        }


        sensor_t *s = esp_camera_sensor_get();
        //initial sensors are flipped vertically and colors are a bit saturated
        s->set_quality(s, 5);
        s->set_vflip(s, 1); //flip it back
        s->set_hmirror(s, 1);
        s->set_framesize(s, _frameSize);

        // s->set_brightness(s, -2);

        _frameBufferCount = frameBufferCount;

        return ESP_OK;
    }

    class URICharacter : public Or
    {
    public:
        URICharacter() : Or(
                             new Range('0', '9'),
                             new Range('a', 'z'),
                             new Range('A', 'Z'),
                             new BeeFishParser::Character('%'))
        {
        }
    };

    class URIWord : public Capture
    {
    public:
        URIWord() : Capture(
                        new Repeat(
                            new URICharacter()))
        {
        }
    };

    class NameValuePair : public And
    {
    private:
        URIWord *_name;
        URIWord *_value;

    public:
        NameValuePair() : And(
                              _name = new URIWord(),
                              new BeeFishParser::Character('='),
                              _value = new URIWord(),
                              new Optional(new BeeFishParser::Character('&')))
        {
        }

        const BString &name()
        {
            return _name->value();
        }

        const BString &value()
        {
            return _value->value();
        }
    };

    class QueryString : public Repeat
    {
    private:
        framesize_t &_frameSize;

    public:
        QueryString(framesize_t &frameSize) : Repeat(new NameValuePair()),
                                              _frameSize(frameSize)
        {
        }

        virtual void matchedItem(Match *match)
        {
            NameValuePair *pair = (NameValuePair *)match;

            if (pair->name() == "size")
            {
                const BString &value = pair->value();
                if (value == "QVGA")
                    _frameSize = FRAMESIZE_QVGA;
                else if (value == "CIF")
                    _frameSize = FRAMESIZE_CIF;
                else if (value == "VGA")
                    _frameSize = FRAMESIZE_VGA;
                else if (value == "SVGA")
                    _frameSize = FRAMESIZE_SVGA;
                else if (value == "XGA")
                    _frameSize = FRAMESIZE_XGA;
                else if (value == "SXGA")
                    _frameSize = FRAMESIZE_SXGA;
                else if (value == "UXGA")
                    _frameSize = FRAMESIZE_UXGA;
            }
        }
    };

public:
    esp_err_t setFrameSize(framesize_t frameSize)
    {
        esp_err_t result = ESP_OK;

        sensor_t *s = esp_camera_sensor_get();

        if (s)
            result = s->set_framesize(s, frameSize);
        else
            result = ESP_ERR_CAMERA_FAILED_TO_SET_FRAME_SIZE;

        if (result == ESP_OK && frameSize != _frameSize)
        {
            for (int i = 0; i < _frameBufferCount; ++i)
            {
                fb = esp_camera_fb_get();
                if (fb)
                {
                    esp_camera_fb_return(fb);
                    fb = nullptr;
                }
            }
        }

        _frameSize = frameSize;

        return result;
    }

    esp_err_t setFrameSize(const BeeFishBString::BString &uri)
    {
        //FRAMESIZE_ QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA

        framesize_t frameSize = FRAMESIZE_SVGA;

        QueryString queryString(frameSize);

        Parser parser(queryString);

        parser.read(uri);

        return setFrameSize(frameSize);
    }

    esp_err_t setFrameBufferCount(size_t frameBufferCount)
    {

        if (frameBufferCount == _frameBufferCount)
            return ESP_OK;

        return initialize(frameBufferCount);
    }
};
