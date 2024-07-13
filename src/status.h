#ifndef VACUUM_STATUS_H_
#define VACUUM_STATUS_H_

enum class Status
{
    FINISHED,
    WORKING,
    DEAD
};

// Adapting Status enum class to be streamable, for logging purposes:
namespace std
{
    /**
     * @brief Overload of the stream operator for the Status enum.
     */
    inline std::ostream& operator<<(std::ostream& ostream, const Status& status)
    {
        std::string status_string;

        switch (status)
        {
            case Status::FINISHED:
                status_string = "FINISHED";
                break;

            case Status::WORKING:
                status_string = "WORKING";
                break;

            case Status::DEAD:
                status_string = "DEAD";
                break;
        }

        ostream << status_string;
        return ostream;
    }
}

#endif /* VACUUM_STATUS_H_ */
