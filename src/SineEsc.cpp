#include "SineEsc.h"

SineEsc::SineEsc(CanardAdapter &adapter)
: CanardAdapterNode(adapter)
{
}

void SineEsc::begin(uint8_t nodeId)
{
    _nodeId = nodeId;
    _beginNode();
}

bool SineEsc::shouldAcceptTransfer(uint64_t* out_data_type_signature,
                                  uint16_t data_type_id,
                                  CanardTransferType transfer_type,
                                  uint8_t source_node_id)
{
    bool accept = false;

    if (transfer_type == CanardTransferTypeResponse && source_node_id == _nodeId) {
        switch(data_type_id)
        {
            case SINE_ESC_SETTHROTTLESETTINGS2_ID:
                *out_data_type_signature = SINE_ESC_SETTHROTTLESETTINGS2_SIGNATURE;
                accept = true;
                break;

            case SINE_ESC_SETROTATIONSPEEDSETTINGS_ID:
                *out_data_type_signature = SINE_ESC_SETROTATIONSPEEDSETTINGS_SIGNATURE;
                accept = true;
                break;

            default:
                break;
        }
    } else if (transfer_type == CanardTransferTypeBroadcast && source_node_id == _nodeId) {
        switch(data_type_id)
        {
            case SINE_ESC_BROADCASTSTATUSINFO_ID:
                *out_data_type_signature = SINE_ESC_BROADCASTSTATUSINFO_SIGNATURE;
                accept = true;
                break;

            default:
                break;
        }
    }

    return accept;
}

void SineEsc::onTransferReceived(CanardRxTransfer* transfer)
{
    CanardTransferType transfer_type = static_cast<CanardTransferType>(transfer->transfer_type);
    uint8_t source_node_id = transfer->source_node_id;
    uint16_t data_type_id = transfer->data_type_id;

    if (transfer_type == CanardTransferTypeResponse && source_node_id == _nodeId) {
        switch(data_type_id)
        {
            case SINE_ESC_SETTHROTTLESETTINGS2_ID:
                _handleSetThrottleSettings2Response(transfer);
                break;

            case SINE_ESC_SETROTATIONSPEEDSETTINGS_ID:
                _handleSetRotationSpeedSettingsResponse(transfer);
                break;

            default:
                break;
        }
    } else if (transfer_type == CanardTransferTypeBroadcast && source_node_id == _nodeId) {
        switch(data_type_id)
        {
            case SINE_ESC_BROADCASTSTATUSINFO_ID:
                _handleBroadcastStatusInfo(transfer);
                break;

            default:
                break;
        }
    }
}

const SineEscModel& SineEsc::getModel(void)
{
    return _model;
}

void SineEsc::clearModelReceiveFlags(void)
{
    _model.hasBroadcastStatusInfo = false;
    _model.hasSetThrottleSettings2Response = false;
    _model.hasSetRotationSpeedSettingsResponse = false;
}

void SineEsc::setRotationSpeedSettings(int32_t speed)
{
    sine_esc_SetRotationSpeedSettingsRequest req = {
            .speed = speed
        };

    uint8_t buffer[SINE_ESC_SETROTATIONSPEEDSETTINGS_REQUEST_MAX_SIZE];
    uint32_t bufferLen = sine_esc_SetRotationSpeedSettingsRequest_encode(&req, buffer);

    CanardTxTransfer transfer = {
            .transfer_type = CanardTransferTypeRequest,
            .data_type_signature = SINE_ESC_SETROTATIONSPEEDSETTINGS_REQUEST_SIGNATURE,
            .data_type_id = SINE_ESC_SETROTATIONSPEEDSETTINGS_REQUEST_ID,
            .inout_transfer_id = &_setRotationSpeedSettingsTransferId,
            .priority = CANARD_TRANSFER_PRIORITY_HIGHEST,
            .payload = buffer,
            .payload_len = static_cast<uint16_t>(bufferLen)
        };

    uint16_t ret = canardRequestOrRespondObj(_canard, _nodeId, &transfer);

    if (ret <= 0) {
        // Something went wrong
    }

    _model.hasSetRotationSpeedSettingsResponse = false;
}

void SineEsc::setThrottleSettings2(uint16_t pwm_us)
{
    sine_esc_SetThrottleSettings2Request req = {
            .pwm_us = pwm_us
        };

    uint8_t buffer[SINE_ESC_SETTHROTTLESETTINGS2_REQUEST_MAX_SIZE];
    uint32_t bufferLen = sine_esc_SetThrottleSettings2Request_encode(&req, buffer);

    CanardTxTransfer transfer = {
            .transfer_type = CanardTransferTypeRequest,
            .data_type_signature = SINE_ESC_SETTHROTTLESETTINGS2_REQUEST_SIGNATURE,
            .data_type_id = SINE_ESC_SETTHROTTLESETTINGS2_REQUEST_ID,
            .inout_transfer_id = &_setThrottleSettings2TransferId,
            .priority = CANARD_TRANSFER_PRIORITY_HIGHEST,
            .payload = buffer,
            .payload_len = static_cast<uint16_t>(bufferLen)
        };

    uint16_t ret = canardRequestOrRespondObj(_canard, _nodeId, &transfer);

    if (ret <= 0) {
        // Something went wrong
    }

    _model.hasSetThrottleSettings2Response = false;
}

void SineEsc::_handleSetThrottleSettings2Response(CanardRxTransfer *transfer)
{
    bool parsed = sine_esc_SetThrottleSettings2Response_decode(
                        transfer,
                        &_model.setThrottleSettings2Response);
    _model.hasSetThrottleSettings2Response = parsed;
}

void SineEsc::_handleSetRotationSpeedSettingsResponse(CanardRxTransfer *transfer)
{
    bool parsed = sine_esc_SetRotationSpeedSettingsResponse_decode(
                        transfer,
                        &_model.setRotationSpeedSettingsResponse);
    _model.hasSetRotationSpeedSettingsResponse = parsed;
}

void SineEsc::_handleBroadcastStatusInfo(CanardRxTransfer *transfer)
{
    bool parsed = sine_esc_BroadcastStatusInfo_decode(
                        transfer,
                        &_model.broadcastStatusInfo);
    _model.hasBroadcastStatusInfo = true;
}
