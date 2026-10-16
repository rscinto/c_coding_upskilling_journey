#include "oled.h"




const uint8_t font5x7[][FONT_WIDTH] = {

    // 0x20 ' '
    {0x00,0x00,0x00,0x00,0x00},

    // 0x21 '!'
    {0x00,0x00,0x5F,0x00,0x00},

    // 0x22 '"'
    {0x00,0x07,0x00,0x07,0x00},

    // 0x23 '#'
    {0x14,0x7F,0x14,0x7F,0x14},

    // 0x24 '$'
    {0x24,0x2A,0x7F,0x2A,0x12},

    // 0x25 '%'
    {0x23,0x13,0x08,0x64,0x62},

    // 0x26 '&'
    {0x36,0x49,0x55,0x22,0x50},

    // 0x27 '''
    {0x00,0x05,0x03,0x00,0x00},

    // 0x28 '('
    {0x00,0x1C,0x22,0x41,0x00},

    // 0x29 ')'
    {0x00,0x41,0x22,0x1C,0x00},

    // 0x2A '*'
    {0x14,0x08,0x3E,0x08,0x14},

    // 0x2B '+'
    {0x08,0x08,0x3E,0x08,0x08},

    // 0x2C ','
    {0x00,0x50,0x30,0x00,0x00},

    // 0x2D '-'
    {0x08,0x08,0x08,0x08,0x08},

    // 0x2E '.'
    {0x00,0x60,0x60,0x00,0x00},

    // 0x2F '/'
    {0x20,0x10,0x08,0x04,0x02},

    // 0x30 '0'
    {0x3E,0x51,0x49,0x45,0x3E},

    // 0x31 '1'
    {0x00,0x42,0x7F,0x40,0x00},

    // 0x32 '2'
    {0x42,0x61,0x51,0x49,0x46},

    // 0x33 '3'
    {0x21,0x41,0x45,0x4B,0x31},

    // 0x34 '4'
    {0x18,0x14,0x12,0x7F,0x10},

    // 0x35 '5'
    {0x27,0x45,0x45,0x45,0x39},

    // 0x36 '6'
    {0x3C,0x4A,0x49,0x49,0x30},

    // 0x37 '7'
    {0x01,0x71,0x09,0x05,0x03},

    // 0x38 '8'
    {0x36,0x49,0x49,0x49,0x36},

    // 0x39 '9'
    {0x06,0x49,0x49,0x29,0x1E},

    // 0x3A ':'
    {0x00,0x36,0x36,0x00,0x00},

    // 0x3B ';'
    {0x00,0x56,0x36,0x00,0x00},

    // 0x3C '<'
    {0x08,0x14,0x22,0x41,0x00},

    // 0x3D '='
    {0x14,0x14,0x14,0x14,0x14},

    // 0x3E '>'
    {0x00,0x41,0x22,0x14,0x08},

    // 0x3F '?'
    {0x02,0x01,0x51,0x09,0x06},

    // 0x40 '@'
    {0x32,0x49,0x79,0x41,0x3E},

    // 0x41 'A'
    {0x7E,0x11,0x11,0x11,0x7E},

    // 0x42 'B'
    {0x7F,0x49,0x49,0x49,0x36},

    // 0x43 'C'
    {0x3E,0x41,0x41,0x41,0x22},

    // 0x44 'D'
    {0x7F,0x41,0x41,0x22,0x1C},

    // 0x45 'E'
    {0x7F,0x49,0x49,0x49,0x41},

    // 0x46 'F'
    {0x7F,0x09,0x09,0x09,0x01},

    // 0x47 'G'
    {0x3E,0x41,0x49,0x49,0x7A},

    // 0x48 'H'
    {0x7F,0x08,0x08,0x08,0x7F},

    // 0x49 'I'
    {0x00,0x41,0x7F,0x41,0x00},

    // 0x4A 'J'
    {0x20,0x40,0x41,0x3F,0x01},

    // 0x4B 'K'
    {0x7F,0x08,0x14,0x22,0x41},

    // 0x4C 'L'
    {0x7F,0x40,0x40,0x40,0x40},

    // 0x4D 'M'
    {0x7F,0x02,0x0C,0x02,0x7F},

    // 0x4E 'N'
    {0x7F,0x04,0x08,0x10,0x7F},

    // 0x4F 'O'
    {0x3E,0x41,0x41,0x41,0x3E},
    // 0x50 'P'
    {0x7F,0x09,0x09,0x09,0x06},

    // 0x51 'Q'
    {0x3E,0x41,0x51,0x21,0x5E},

    // 0x52 'R'
    {0x7F,0x09,0x19,0x29,0x46},

    // 0x53 'S'
    {0x46,0x49,0x49,0x49,0x31},

    // 0x54 'T'
    {0x01,0x01,0x7F,0x01,0x01},

    // 0x55 'U'
    {0x3F,0x40,0x40,0x40,0x3F},

    // 0x56 'V'
    {0x1F,0x20,0x40,0x20,0x1F},

    // 0x57 'W'
    {0x7F,0x20,0x18,0x20,0x7F},

    // 0x58 'X'
    {0x63,0x14,0x08,0x14,0x63},

    // 0x59 'Y'
    {0x03,0x04,0x78,0x04,0x03},

    // 0x5A 'Z'
    {0x61,0x51,0x49,0x45,0x43},

    // 0x5B '['
    {0x00,0x7F,0x41,0x41,0x00},

    // 0x5C '\'
    {0x02,0x04,0x08,0x10,0x20},

    // 0x5D ']'
    {0x00,0x41,0x41,0x7F,0x00},

    // 0x5E '^'
    {0x04,0x02,0x01,0x02,0x04},

    // 0x5F '_'
    {0x40,0x40,0x40,0x40,0x40},

    // 0x60 '`'
    {0x00,0x01,0x02,0x04,0x00},

    // 0x61 'a'
    {0x20,0x54,0x54,0x54,0x78},

    // 0x62 'b'
    {0x7F,0x48,0x44,0x44,0x38},

    // 0x63 'c'
    {0x38,0x44,0x44,0x44,0x20},

    // 0x64 'd'
    {0x38,0x44,0x44,0x48,0x7F},

    // 0x65 'e'
    {0x38,0x54,0x54,0x54,0x18},

    // 0x66 'f'
    {0x08,0x7E,0x09,0x01,0x02},

    // 0x67 'g'
    {0x0C,0x52,0x52,0x52,0x3E},

    // 0x68 'h'
    {0x7F,0x08,0x04,0x04,0x78},

    // 0x69 'i'
    {0x00,0x44,0x7D,0x40,0x00},

    // 0x6A 'j'
    {0x20,0x40,0x44,0x3D,0x00},

    // 0x6B 'k'
    {0x7F,0x10,0x28,0x44,0x00},

    // 0x6C 'l'
    {0x00,0x41,0x7F,0x40,0x00},

    // 0x6D 'm'
    {0x7C,0x04,0x18,0x04,0x78},

    // 0x6E 'n'
    {0x7C,0x08,0x04,0x04,0x78},

    // 0x6F 'o'
    {0x38,0x44,0x44,0x44,0x38},

    // 0x70 'p'
    {0x7C,0x14,0x14,0x14,0x08},

    // 0x71 'q'
    {0x08,0x14,0x14,0x18,0x7C},

    // 0x72 'r'
    {0x7C,0x08,0x04,0x04,0x08},

    // 0x73 's'
    {0x48,0x54,0x54,0x54,0x20},

    // 0x74 't'
    {0x04,0x3F,0x44,0x40,0x20},

    // 0x75 'u'
    {0x3C,0x40,0x40,0x20,0x7C},

    // 0x76 'v'
    {0x1C,0x20,0x40,0x20,0x1C},

    // 0x77 'w'
    {0x3C,0x40,0x30,0x40,0x3C},

    // 0x78 'x'
    {0x44,0x28,0x10,0x28,0x44},

    // 0x79 'y'
    {0x0C,0x50,0x50,0x50,0x3C},

    // 0x7A 'z'
    {0x44,0x64,0x54,0x4C,0x44},
};


uint8_t init[] = {
    0x00,
    0xAE,       // display off
    0xD5, 0x80, // clock divide
    0xA8, 0x3F, // multiplex 1/64
    0xD3, 0x00, // display offset
    0x40,       // start line
    0x8D, 0x14, // charge pump ON
    0x20, 0x02, // 0x00 horizontal addressing, 0x02 page Addressing
    0xA1,       // segment remap
    0xC8,       // COM scan direction
    0xDA, 0x12, // COM pins config
    0x81, 0xCF, // contrast
    0xD9, 0xF1, // pre-charge
    0xDB, 0x40, // VCOM detect
    0xA4,       // resume RAM display
    0xA6,       // normal display
    0xAF        // display ON
  };


  uint8_t on[]      = {0x00, 0xAF};
  uint8_t all_on[]  = {0x00, 0xA5};
  uint8_t ram_mode[] = {0x00, 0xA4};


  void OLED_draw_bitmap(OLED_Handle_t *oled, const uint8_t *bitmap)
    {
        uint8_t data[17];

        data[0] = 0x40;

        for (uint8_t page = 0; page < 8; page++)
        {
            OLED_set_cursor(oled,page, 0);

            for (uint8_t chunk = 0; chunk < 8; chunk++)
            {
                for (uint8_t i = 0; i < 16; i++)
                {
                    data[i + 1] =
                        bitmap[(page * 128) + (chunk * 16) + i];
                }

                HAL_I2C_Master_Transmit(oled->i2c,
                                        SSD1306_ADDR << 1,
                                        data,
                                        sizeof(data),
                                        HAL_MAX_DELAY);
            }
        }
    }






  void OLED_print(OLED_Handle_t *oled, char *str)
  {
      while (*str != '\0')
      {
          OLED_draw_char(oled, *str);
          str++;
      }
  }





  void OLED_screen_test(OLED_Handle_t *oled)
  {
      uint8_t data[129];

      data[0] = 0x40;   // data control byte

      // Fill screen white
      for (int i = 1; i < sizeof(data); i++)
      {
          data[i] = 0xFF;
      }

      OLED_set_cursor(oled, 0, 0);

      for (int page = 0; page < 8; page++)
      {
          OLED_set_cursor(oled, page, 0);

          HAL_I2C_Master_Transmit(oled->i2c,
                                  SSD1306_ADDR << 1,
                                  data,
                                  sizeof(data),
                                  HAL_MAX_DELAY);
      }

      HAL_Delay(2000);

      OLED_clear(oled);

      HAL_Delay(2000);
  }


  void OLED_init(OLED_Handle_t *oled, I2C_HandleTypeDef *hi2c)
  {
      oled->i2c = hi2c;
      oled->address = SSD1306_ADDR;
      oled->dma_busy = false;
      oled->needs_redraw = true;
      oled->name = "OLED";

      OLED_clear_framebuffer(oled);

      HAL_I2C_Master_Transmit(oled->i2c,
                              oled->address << 1,
                              init,
                              sizeof(init),
                              HAL_MAX_DELAY);

      OLED_clear(oled);
  }

  void OLED_draw_char(OLED_Handle_t *oled, char c)
    {
        if (c < FONT_FIRST_CHAR  || c > FONT_LAST_CHAR)   // for now, based on your current table
        {
            c = '?';
        }

        uint8_t index = c - 0x20;

        uint8_t data[7];
        data[0] = 0x40;

        for (int i = 0; i < FONT_WIDTH; i++)
        {
            data[i + 1] = font5x7[index][i];
        }

        data[6] = 0x00;   // spacing

        HAL_I2C_Master_Transmit(oled->i2c, SSD1306_ADDR << 1,
                                data, sizeof(data), HAL_MAX_DELAY);
    }



  void OLED_draw_char_by_index(OLED_Handle_t *oled, uint8_t index)
  {
      uint8_t data[7];

      data[0] = 0x40;

      for (int i = 0; i < FONT_WIDTH; i++)
      {
          data[i + 1] = font5x7[index][i];
      }

      data[6] = 0x00;

      HAL_I2C_Master_Transmit(oled->i2c, SSD1306_ADDR << 1, data, sizeof(data), HAL_MAX_DELAY);
  }

  void OLED_set_cursor(OLED_Handle_t *oled, uint8_t page, uint8_t col)
  {
	  uint8_t set_pos[] = {
	      0x00,
	      0xB0 | page,       // page 0
	      0x00 | (col & 0x0F),       // lower column nibble
	      0x10 | ((col >> 4) & 0x0F)        // upper column nibble
	  };

	  HAL_I2C_Master_Transmit(oled->i2c,
	                          oled->address << 1,
	                          set_pos,
	                          sizeof(set_pos),
	                          50);
  }

  void OLED_clear(OLED_Handle_t *oled)
  {
	    uint8_t clear[129];
	    clear[0] = 0x40;

	    for (int i = 1; i < 129; i++)
	    {
	        clear[i] = 0x00;
	    }

	    for (uint8_t page = 0; page < 8; page++)
	    {
	        OLED_set_cursor(oled, page, 0);

	        HAL_I2C_Master_Transmit(
				oled->i2c,
	            SSD1306_ADDR << 1,
	            clear,
	            sizeof(clear),
	            HAL_MAX_DELAY
	        );
	    }

	    OLED_set_cursor(oled, 0, 0);
  }


  void OLED_clear_framebuffer(OLED_Handle_t *oled)
  {
      memset(oled->framebuffer, 0x00, sizeof(oled->framebuffer));
  }

  void OLED_draw_pixel(OLED_Handle_t *oled, uint8_t x, uint8_t y)
  {
      if (x >= 128 || y >= 64)
      {
          return;
      }

      uint16_t index = x + (y / 8) * 128;
      uint8_t bit = 1 << (y % 8);

      oled->framebuffer[index] |= bit;
  }

  static void OLED_draw_char_fb(OLED_Handle_t *oled, uint8_t x, uint8_t y, char c)
  {
      if (c < FONT_FIRST_CHAR || c > FONT_LAST_CHAR)
      {
          c = '?';
      }

      uint8_t index = c - FONT_FIRST_CHAR;

      for (uint8_t col = 0; col < FONT_WIDTH; col++)
      {
          uint8_t column_bits = font5x7[index][col];

          for (uint8_t row = 0; row < 7; row++)
          {
              if (column_bits & (1 << row))
              {
                  OLED_draw_pixel(oled, x + col, y + row);
              }
          }
      }
  }

  void OLED_draw_text_fb(OLED_Handle_t *oled, uint8_t x, uint8_t y, const char *str)
  {
      while (*str != '\0')
      {
          OLED_draw_char_fb(oled, x, y, *str);
          x += 6;

          if (x > 122)
          {
              break;
          }

          str++;
      }
  }

  void OLED_flush_framebuffer(OLED_Handle_t *oled)
  {
      uint8_t data[129];

      data[0] = 0x40;

      for (uint8_t page = 0; page < 8; page++)
      {
          OLED_set_cursor(oled, page, 0);

          for (uint8_t col = 0; col < 128; col++)
          {
              data[col + 1] = oled->framebuffer[(page * 128) + col];
          }

          HAL_StatusTypeDef status =
              HAL_I2C_Master_Transmit(oled->i2c,
                                      oled->address << 1,
                                      data,
                                      sizeof(data),
                                      50);

          if (status != HAL_OK)
          {
              return;
          }
      }
  }




  static void OLED_draw_rect_fb(OLED_Handle_t *oled,
                                uint8_t x,
                                uint8_t y,
                                uint8_t w,
                                uint8_t h)
  {
      if (oled == NULL)
      {
          return;
      }

      if (w == 0 || h == 0)
      {
          return;
      }

      /*
       * Clamp so we do not draw outside the 128x64 screen.
       */
      if (x >= 128 || y >= 64)
      {
          return;
      }

      if ((x + w) > 128)
      {
          w = 128 - x;
      }

      if ((y + h) > 64)
      {
          h = 64 - y;
      }

      /*
       * Top and bottom horizontal lines.
       */
      for (uint8_t i = 0; i < w; i++)
      {
          OLED_draw_pixel(oled, x + i, y);

          if (h > 1)
          {
              OLED_draw_pixel(oled, x + i, y + h - 1);
          }
      }

      /*
       * Left and right vertical lines.
       */
      for (uint8_t i = 0; i < h; i++)
      {
          OLED_draw_pixel(oled, x, y + i);

          if (w > 1)
          {
              OLED_draw_pixel(oled, x + w - 1, y + i);
          }
      }
  }


  void OLED_draw_graph_fb(OLED_Handle_t *oled,
                          const Graph_Buffer_t *graph,
                          uint8_t x,
                          uint8_t y,
                          uint8_t w,
                          uint8_t h,
                          float min_value,
                          float max_value)
  {
      if (oled == NULL || graph == NULL)
      {
          return;
      }

      if (w == 0 || h == 0)
      {
          return;
      }

      if (max_value <= min_value)
      {
          return;
      }

      OLED_draw_rect_fb(oled, x, y, w, h);

      if (graph->count == 0)
      {
          return;
      }

      /*
       * Leave 1-pixel border around the graph.
       */
      uint8_t graph_x = x + 1;
      uint8_t graph_y = y + 1;
      uint8_t graph_w = w - 2;
      uint8_t graph_h = h - 2;

      if (graph_w == 0 || graph_h == 0)
      {
          return;
      }

      for (uint8_t pixel_x = 0; pixel_x < graph_w; pixel_x++)
      {
          uint16_t history_x;

          if (graph_w >= GRAPH_HISTORY_LEN)
          {
              history_x = pixel_x;
          }
          else
          {
              history_x = (uint16_t)((pixel_x * GRAPH_HISTORY_LEN) / graph_w);
          }

          if (history_x >= GRAPH_HISTORY_LEN)
          {
              history_x = GRAPH_HISTORY_LEN - 1;
          }

          uint16_t sample_index = graph_buffer_get_ordered_index(graph, history_x);

          if (!graph->valid[sample_index])
          {
              continue;
          }

          float value = graph->samples[sample_index];

          if (value < min_value)
          {
              value = min_value;
          }

          if (value > max_value)
          {
              value = max_value;
          }

          float normalized = (value - min_value) / (max_value - min_value);

          uint8_t pixel_y = graph_y + graph_h - 1 -
                            (uint8_t)(normalized * (float)(graph_h - 1));

          OLED_draw_pixel(oled, graph_x + pixel_x, pixel_y);
      }
  }
