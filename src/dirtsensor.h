#ifndef VACUUM_DIRTSENSOR_H_
#define VACUUM_DIRTSENSOR_H_

class DirtSensor {
    public:
        virtual unsigned int getDirtLevel() const = 0;
};

#endif /* VACUUM_DIRTSENSOR_H_ */