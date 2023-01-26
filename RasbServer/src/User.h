#include <string>

class User
{
    public:
        User(const std::string& adress, const int& id)
            : m_Adress(adress), m_Id(id) {}
        ~User() = default;

    public:
        const std::string& GetAdress() {return m_Adress;}
        const int& GetId() {return m_Id;}

    private:
        std::string m_Adress;
        int m_Id;
};