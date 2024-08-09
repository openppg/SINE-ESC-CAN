#ifndef SINE_ESC_H_
#define SINE_ESC_H_

#include <CanardAdapter.h>
#include <dronecan_msgs.h>
#include <stdint.h>

struct SineEscModel
{
    bool hasBroadcastStatusInfo;
    sine_esc_BroadcastStatusInfo broadcastStatusInfo;

    bool hasSetThrottleSettings2Response;
    sine_esc_SetThrottleSettings2Response setThrottleSettings2Response;

    bool hasSetRotationSpeedSettingsResponse;
    sine_esc_SetRotationSpeedSettingsResponse setRotationSpeedSettingsResponse;
};

class SineEsc : public CanardAdapterNode
{
public:
    SineEsc(CanardAdapter &adapter);

    void begin(uint8_t nodeId);

    bool shouldAcceptTransfer(uint64_t* out_data_type_signature,
                                      uint16_t data_type_id,
                                      CanardTransferType transfer_type,
                                      uint8_t source_node_id) override;

    void onTransferReceived(CanardRxTransfer* transfer) override;

    const SineEscModel& getModel(void);
    void clearModelReceiveFlags(void);

    void setRotationSpeedSettings(int32_t speed);

    void setThrottleSettings2(uint16_t pwm_us);

private:
    uint8_t _nodeId;
    uint8_t _setThrottleSettings2TransferId;
    uint8_t _setRotationSpeedSettingsTransferId;
    SineEscModel _model;

    void _handleSetThrottleSettings2Response(CanardRxTransfer *transfer);
    void _handleSetRotationSpeedSettingsResponse(CanardRxTransfer *transfer);
    void _handleBroadcastStatusInfo(CanardRxTransfer *transfer);
};

#endif // SINE_ESC_H_
