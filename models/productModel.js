const mongoose = require("mongoose");
mongoose.set('strictQuery', true);  

const productSchema = mongoose.Schema({
    x: {type: Number },
    y: {type: Number },
    z: {type: Number },
});

module.exports = mongoose.model("Product", productSchema);
