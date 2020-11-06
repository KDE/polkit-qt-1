#ifndef POLKITQT1_DETAILS_P_H
#define POLKITQT1_DETAILS_P_H

#include <polkitqt1-details.h>

#include <QSharedData>

#include <polkit/polkit.h>

/**
  * \internal
  */
class PolkitQt1::Details::Data : public QSharedData
{
public:
    Data() {}
    Data(const Data &other)
        : QSharedData(other)
        , polkitDetails(other.polkitDetails)
    {
        if (polkitDetails != nullptr) {
            g_object_ref(polkitDetails);
        }
    }
    ~Data()
    {
        if (polkitDetails != nullptr) {
            g_object_unref(polkitDetails);
        }
    }

    PolkitDetails *polkitDetails;
};

#endif // DETAILS_P_H
