#ifndef VACUUM_STATUS_H_
#define VACUUM_STATUS_H_

enum class Status
{
    Finished,
    Working,
    Dead
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
            case Status::Finished:
                status_string = "FINISHED";
                break;

            case Status::Working:
                status_string = "WORKING";
                break;

            case Status::Dead:
                status_string = "DEAD";
                break;
        }

        ostream << status_string;
        return ostream;
    }
}

#endif /* VACUUM_STATUS_H_ */
