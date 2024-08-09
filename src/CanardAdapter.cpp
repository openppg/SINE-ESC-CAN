#include "CanardAdapter.h"

#include <algorithm>
#include <string.h>

// -----------------------------------------------------------------------------

static uint64_t getTimestamp(void)
{
    static uint64_t baseTime = 0;
    static unsigned long lastMicros = 0;

    unsigned long now = micros();
    if (now < lastMicros) {
        // micros() overflowed at 32-bit max value
        baseTime += 0x100000000ULL;
    }

    lastMicros = now;

    return (now + baseTime);
}

// -----------------------------------------------------------------------------

CanardAdapterNode::CanardAdapterNode(CanardAdapter &adapter)
: _canard(NULL)
, _adapter(&adapter)
{
}

void CanardAdapterNode::_beginNode(void)
{
    _canard = &_adapter->_canard;
    _adapter->_addNode(*this);
}

// -----------------------------------------------------------------------------

CanardAdapter::CanardAdapter(Adafruit_MCP2515 &mcp)
: _mcp(&mcp)
{
}

void CanardAdapter::begin(uint8_t *memoryPool, size_t memoryPoolSize)
{
    canardInit(&_canard,
               memoryPool,
               memoryPoolSize,
               _onTransferReceivedTrampoline,
               _shouldAcceptTransferTrampoline,
               this);
}

void CanardAdapter::setLocalNodeId(uint8_t selfNodeId)
{
    canardSetLocalNodeID(&_canard, selfNodeId);
}

void CanardAdapter::processTxRxOnce(void)
{
    // Transmitting
    for (const CanardCANFrame* txf = NULL; (txf = canardPeekTxQueue(&_canard)) != NULL;) {
        _mcp->beginExtendedPacket(txf->id);
        _mcp->write(txf->data, txf->data_len);

        int txRes = _mcp->endPacket();
        if (txRes > 0) {
            // Success, remove the packet from the queue
            canardPopTxQueue(&_canard);
        } else {
            // Timeout, exit and try again later
            break;
        }
    }

    // Receiving
    const uint64_t timestamp = getTimestamp();

    CanardCANFrame rxFrame;
    memset(&rxFrame, 0, sizeof(rxFrame));

    rxFrame.data_len = std::min<int>(_mcp->parsePacket(), sizeof(rxFrame.data));
    if (rxFrame.data_len > 0) {
        rxFrame.id = _mcp->packetId();

        for (int i = 0; i < rxFrame.data_len; i++) {
            rxFrame.data[i] = _mcp->read();
        }

        canardHandleRxFrame(&_canard, &rxFrame, timestamp);
    }
}


void CanardAdapter::_onTransferReceivedTrampoline(CanardInstance* instance,
                                                  CanardRxTransfer* transfer)
{
    CanardAdapter* adapter = static_cast<CanardAdapter*>(instance->user_reference);
    adapter->_onTransferReceived(transfer);
}

bool CanardAdapter::_shouldAcceptTransferTrampoline(const CanardInstance* instance,
                                                    uint64_t* out_data_type_signature,
                                                    uint16_t data_type_id,
                                                    CanardTransferType transfer_type,
                                                    uint8_t source_node_id)
{
    CanardAdapter* adapter = static_cast<CanardAdapter*>(instance->user_reference);
    return adapter->_shouldAcceptTransfer(out_data_type_signature,
                                          data_type_id,
                                          transfer_type,
                                          source_node_id);
}


void CanardAdapter::_addNode(CanardAdapterNode &node)
{
    _nodes.push_back(&node);
}

bool CanardAdapter::_shouldAcceptTransfer(uint64_t* out_data_type_signature,
                                          uint16_t data_type_id,
                                          CanardTransferType transfer_type,
                                          uint8_t source_node_id)
{
    bool accept = false;

    for (CanardAdapterNode *node : _nodes) {
        if (node->shouldAcceptTransfer(out_data_type_signature,
                                       data_type_id,
                                       transfer_type,
                                       source_node_id)) {
            accept = true;
            break;
        }
    }

    return accept;
}

void CanardAdapter::_onTransferReceived(CanardRxTransfer* transfer)
{
    for (CanardAdapterNode *node : _nodes) {
        node->onTransferReceived(transfer);
    }
}
