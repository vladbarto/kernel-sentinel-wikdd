import './App.css';
import * as React from 'react';
import Box from '@mui/material/Box';
import Card from '@mui/material/Card';
import CardActions from '@mui/material/CardActions';
import CardContent from '@mui/material/CardContent';
import Button from '@mui/material/Button';
import Typography from '@mui/material/Typography';

const bull = (
  <Box
    component="span"
    sx={{ display: 'inline-block', mx: '2px', transform: 'scale(0.8)' }}
  >
    •
  </Box>
);

// const card = (
//   <React.Fragment>
//     <CardContent>
//       {/* <Typography gutterBottom sx={{ color: 'text.secondary', fontSize: 14 }}>
//         Word of the Day
//       </Typography>
//       <Typography variant="h5" component="div">
//         be{bull}nev{bull}o{bull}lent
//       </Typography>
//       <Typography sx={{ color: 'text.secondary', mb: 1.5 }}>adjective</Typography>
//       <Typography variant="body2">
//         well meaning and kindly.
//         <br />
//         {'"a benevolent smile"'}
//       </Typography> */}
//       <Typography>
//         <CardActions>
//           <Button size="small">create driver</Button>
//         </CardActions>
//         <CardActions>
//           <Button size="small">start driver</Button>
//         </CardActions>
//         <CardActions>
//           <Button size="small">stop driver</Button>
//         </CardActions>
//         <CardActions>
//           <Button size="small">delete driver</Button>
//         </CardActions>
//       </Typography>
//     </CardContent>
//     <CardActions>
//       <Button size="small">Learn More</Button>
//     </CardActions>
//   </React.Fragment>
// );

export default function App() {

  return (
    <div style={{display: 'flex',  justifyContent:'center', alignItems:'center', height: '100vh'}}>
      <Box sx={{ minWidth: 275 }}>
        <Typography>
        <CardActions>
          <Button size="small">create driver</Button>
        </CardActions>
        <CardActions>
          <Button size="small">start driver</Button>
        </CardActions>
        <CardActions>
          <Button size="small">stop driver</Button>
        </CardActions>
        <CardActions>
          <Button size="small">delete driver</Button>
        </CardActions>
      </Typography>
      </Box>
    </div>
  );
}