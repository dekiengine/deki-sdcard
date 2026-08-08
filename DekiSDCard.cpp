#include "DekiSDCard.h"
#include "DekiLogSystem.h"

DekiSDCard::Factory DekiSDCard::s_Factory = nullptr;

void DekiSDCard::SetFactory(Factory factory)
{
    s_Factory = factory;
    DEKI_LOG_INTERNAL("DekiSDCard: Factory registered");
}

IDekiSDCard* DekiSDCard::Create()
{
    if (!s_Factory)
    {
        DEKI_LOG_ERROR("DekiSDCard: No factory registered - platform must call SetFactory()");
        return nullptr;
    }
    return s_Factory();
}

bool DekiSDCard::HasFactory()
{
    return s_Factory != nullptr;
}
