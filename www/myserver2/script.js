const errorMessage = document.getElementById("error-message");
const loadBtn = document.getElementById("load-btn");

async function fetchQuotes() {
    // Create a loading message element
    errorMessage.textContent = "";
    try {
        const response = await fetch("https://dummyjson.com/quotes/random");
        const q = await response.json();

        document.getElementById("quote-text").textContent = q.quote;
        document.getElementById("quote-author").textContent = q.author;
    } catch (error) {
        // Replace the loading message with an error message

        errorMessage.textContent = "😱 Aiuto! Couldn't load quotes. The philosopher servers are napping.";

        console.error(error);
    }
}

// Load on startup
fetchQuotes();

// Load on button click
loadBtn.addEventListener("click", fetchQuotes);
