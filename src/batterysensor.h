#ifndef VACUUM_BATTERYSENSOR_H_
#define VACUUM_BATTERYSENSOR_H_

class BatterySensor
{
    public:
        virtual float getCurrentAmount() const = 0;
};

#endif /* VACUUM_BATTERYSENSOR_H_ */
