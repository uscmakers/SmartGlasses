const express = require('express')
const uuid = require('uuid')
const dialogflow = require('@google-cloud/dialogflow')

const app = express()

const PORT = process.env.PORT || 3000

app.use(express.json())

const projectId = 'makerssmartglasses'

const sessionClient = new dialogflow.SessionsClient({
    projectId,
    keyFilename: './service-account.json',
})

// A unique identifier for the given session
const sessionId = uuid.v4()

async function detectIntent(text) {
    // Create a new session
    const sessionPath = sessionClient.projectAgentSessionPath(
        projectId,
        sessionId
    )

    // The text query request.
    const request = {
        session: sessionPath,
        queryInput: {
            text: {
                // The query to send to the dialogflow agent
                text: text,
                // The language used by the client (en-US)
                languageCode: 'en-US',
            },
        },
    }

    // Send request and log result
    return await sessionClient.detectIntent(request)
}

app.post('/', async (req, res) => {
    const text = req.body.text
    const responses = await detectIntent(text)
    console.dir(responses, { depth: null })
    console.log('Detected intent')
    const result = responses[0].queryResult
    console.log(`  Query: ${result.queryText}`)
    console.log(`  Response: ${result.fulfillmentText}`)
    if (result.intent) {
        console.log(`Intent: ${result.intent.displayName}`)
    } else {
        console.log(`No intent matched.`)
    }
    res.json(responses)
})

app.listen(PORT, () => {
    console.log(`App running on port: ${PORT}`)
})
