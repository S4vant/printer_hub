#include "JournalReader.h"
#include "JobParser.h"
#include "JsonWriter.h"

int main()
{
    JournalReader reader;

    auto messages =
        reader.readMessages();

    JobParser parser;

    auto jobs =
        parser.parse(messages);

    JsonWriter writer;

    writer.save(jobs);

    return 0;
}