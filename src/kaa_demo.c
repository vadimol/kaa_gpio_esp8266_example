/**
 *  Copyright 2014-2016 CyberVision, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <kaa_error.h>
#include <kaa_context.h>
#include <platform/kaa_client.h>
#include <utilities/kaa_log.h>
#include <utilities/kaa_mem.h>
#include <kaa_user.h>
#include <gen/kaa_remote_control_ecf.h>
#include "target_gpio_led.h"
#include "target.h"

#define DEMO_USER "user"
#define DEMO_TOKEN "token"

static kaa_client_t *kaa_client = NULL;

static int gpio_led[NUM_GPIO_LEDS];

/*
 * Event callback-s.
 */
static void kaa_device_info_request(void *context
                           , kaa_remote_control_ecf_device_info_request_t *event
                           , kaa_endpoint_id_p source)
{
    (void)context;
    (void)source;

    kaa_remote_control_ecf_device_info_response_t *response = kaa_remote_control_ecf_device_info_response_create();

    response->device_name = kaa_string_copy_create(TARGET_DEVICE_NAME);
    response->model       = kaa_string_copy_create(TARGET_MODEL_NAME);
    response->gpio_status = kaa_list_create();

    for (int i = 0; i < NUM_GPIO_LEDS; ++i) {
        kaa_remote_control_ecf_gpio_status_t *gio_status = kaa_remote_control_ecf_gpio_status_create();
        gio_status->id = i;
        gio_status->status = gpio_led[i];
        kaa_list_push_back(response->gpio_status, (void*)gio_status);
    }

    kaa_event_manager_send_kaa_remote_control_ecf_device_info_response(kaa_client_get_context(kaa_client)->event_manager, response, NULL);

    response->destroy(response); // Destroying event that was successfully sent
    event->destroy(event);
}

static void kaa_gpio_toggle_request(void *context
                              , kaa_remote_control_ecf_gpio_toggle_request_t *event
                              , kaa_endpoint_id_p source)
{
    (void)context;
    (void)source;

    if (event->gpio->id >= NUM_GPIO_LEDS) {
    	event->destroy(event);
    	return;
    }

    demo_printf("Toggling GPIO: id = %d...\r\n", event->gpio->id);

    target_gpio_led_toggle(event->gpio->id, event->gpio->status);
    gpio_led[event->gpio->id] = event->gpio->status;

    event->destroy(event);
}

int main(void)
{
    int rc = target_initialize();
    if (rc < 0) {
        return 1;
    }

    target_gpio_led_init();

    /**
     * Initialize Kaa client.
     */
    kaa_error_t error_code = kaa_client_create(&kaa_client, NULL);
    if (error_code) {
    	return error_code;
    }

    error_code = kaa_profile_manager_set_endpoint_access_token(kaa_client_get_context(kaa_client)->profile_manager,
            DEMO_TOKEN);

    error_code = kaa_user_manager_default_attach_to_user(kaa_client_get_context(kaa_client)->user_manager
                                                        , DEMO_USER
                                                        , DEMO_TOKEN);
    if (error_code) {
        goto exit;
    }

    error_code = kaa_event_manager_set_kaa_remote_control_ecf_device_info_request_listener(kaa_client_get_context(kaa_client)->event_manager,
            kaa_device_info_request,
            NULL);

    if (error_code) {
        goto exit;
    }

    error_code = kaa_event_manager_set_kaa_remote_control_ecf_gpio_toggle_request_listener(kaa_client_get_context(kaa_client)->event_manager,
    		kaa_gpio_toggle_request,
            NULL);

    if (error_code) {
        goto exit;
    }

    demo_printf("Start Kaa\r\n");
    /**
     * Start Kaa client main loop.
     */
    error_code = kaa_client_start(kaa_client, 0, NULL, 0);
    if (error_code) {
        goto exit;
    }

exit:
    /**
     * Destroy Kaa client.
     */
    kaa_client_destroy(kaa_client);

    return error_code;
}
