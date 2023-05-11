// require('dotenv').config();

const express = require('express');
const mongoose = require('mongoose');
const mongoString = "mongodb+srv://munayassiin:aliyare5@cluster0.fpjvhlj.mongodb.net/?retryWrites=true&w=majority";
const routes = require('./routes/products');
const ProductModel = require("./models/productModel");

mongoose.connect( mongoString );
const database = mongoose.connection;

database.on('error', (error) => {
    console.log(error)
})

database.once('connected', () => {
    console.log('Database Connected');
})
const app = express();
app.use(express.urlencoded({ extended: true }));
app.use(express.json());
app.use('/api',routes)


/*
async function checkForDataInDatabase() {
  const products = await ProductModel.find();
  // If nothing in the database, let's just add some?
  if (products.length == 1) {
    const productInstance1 = new ProductModel({
      x:5,
      y:5,
      z:4,
    });

    await productInstance1.save();

    console.log("Added 1 product since the database was empty!");
  } else {
    console.log("products", products.length);
  }
}
checkForDataInDatabase();
*/

app.listen(3001)