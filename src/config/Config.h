class Config
{
public:
    bool load(const std::string& path);

    std::string serverUrl() const;

private:
    std::string m_serverUrl;
};