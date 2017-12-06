#!/usr/bin/env python3

import argparse
import operator
import fractions

COMMAND_ENCRYPT = 'encrypt'
COMMAND_DECRYPT = 'decrypt'
COMMAND_BREAK = 'break'

ENGLISH_ALPHABET = 'abcdefghijklmnopqrstuvwxyz'
RUSSIAN_ALPHABET = 'абвгдеёжзийклмнопрстуфхцчшщъыьэюя'
ALPHABETS = {
    'en': ENGLISH_ALPHABET,
    'ru': RUSSIAN_ALPHABET,
}

def inverse_alphabet(alphabet):
    res = {}
    for id, letter in enumerate(alphabet):
        res[letter] = id
    return res

def encrypt(args):
    text = None
    with open(args.input_file) as f:
        text = f.read()
    key = args.key
    alphabet = ALPHABETS[args.language]

    res = ''
    cur_char_id = 0
    inversed_alphabet = inverse_alphabet(alphabet)

    for c in text:
        if c.lower() in alphabet:
            shift = inversed_alphabet[key[cur_char_id]]
            need_up = False
            if c.isupper():
                c = c.lower()
                need_up = True
            new_id = (inversed_alphabet[c] + shift) % len(alphabet)
            newc = alphabet[new_id]
            if need_up:
                newc = newc.upper()
            res += newc
        else:
            res += c
        cur_char_id += 1
        if cur_char_id == len(key):
            cur_char_id = 0

    with open(args.output_file, 'w') as f:
        f.write(res)

def decrypt(args):
    text = None
    with open(args.input_file) as f:
        text = f.read()
    key = args.key
    alphabet = ALPHABETS[args.language]

    res = ''
    cur_char_id = 0
    inversed_alphabet = inverse_alphabet(alphabet)

    for c in text:
        if c.lower() in alphabet:
            need_up = False
            if c.isupper():
                need_up = True
                c = c.lower()
            shift = inversed_alphabet[key[cur_char_id]]
            new_id = (inversed_alphabet[c] - shift) % len(alphabet)
            newc = alphabet[new_id]
            if need_up:
                newc = newc.upper()
            res += newc
        else:
            res += c
        cur_char_id += 1
        if cur_char_id == len(key):
            cur_char_id = 0

    with open(args.output_file, 'w') as f:
        f.write(res)

def shift(s, id):
    return s[id:] + s[:id]

def frequency_decryption(text, alphabet):
    letter_counts = {}
    for c in alphabet:
        letter_counts[c] = 0
    total_cnt = 0
    for c in text:
        if c.lower() in letter_counts:
            letter_counts[c.lower()] += 1
            total_cnt += 1

    sorted_letters = sorted(list(letter_counts.items()), reverse=True,
        key=lambda x: x[1])

    letters_seq = ''.join(map(lambda a: a[0], sorted_letters))
    initial_id = alphabet.find('e')
    cur_id = alphabet.find(sorted_letters[0][0])
    shifted_alphabet = shift(alphabet, cur_id - initial_id)
    trans = str.maketrans(shifted_alphabet, alphabet)
    result_text = ''
    for c in text:
        need_up = False
        if c.isupper():
            need_up = True
            c = c.lower()
        if c in alphabet:
            c = c.translate(trans)
        if need_up:
            c = c.upper()
        result_text += c
    return result_text

def break_encryption(args):
    text = None
    with open(args.input_file) as f:
        text = f.read()
    alphabet = ALPHABETS[args.language]
    l = args.kasiski_length

    res = ''
    ngrams_count = {}

    for i in range(len(text) - l + 1):
        ngram = text[i:i+l].lower()
        if ngram in ngrams_count:
            ngrams_count[ngram] += 1
        else:
            ngrams_count[ngram] = 1

    max_ngram, max_count = max(ngrams_count.items(), key=operator.itemgetter(1))
    print('Found max ngram "{}" with count = {}'.format(max_ngram, max_count))

    prev_id = text.find(max_ngram)
    dists = set()
    for i in range(prev_id + 1, len(text)):
        if text[i:i+l] == max_ngram:
            dists.add(i - prev_id)
            prev_id = i
    key_length = 0
    for d in dists:
        key_length = fractions.gcd(key_length, d)
    print('Deduced key length: {}'.format(key_length))
    alphabet = None
    if args.language == 'en':
        alphabet = ENGLISH_ALPHABET
    else:
        alphabet = RUSSIAN_ALPHABET

    seqs = []
    for i in range(key_length):
        seqs.append(frequency_decryption(text[i::key_length], alphabet))

    for i in range(len(text)):
        id = i % key_length
        res += seqs[id][i // key_length]

    with open(args.output_file, 'w') as f:
        f.write(res)

def add_file_arguments(parser):
    parser.add_argument('input_file', metavar='INPUT', type=str,
        help='input file name')
    parser.add_argument('output_file', metavar='OUTPUT', type=str,
        help='output file name')

def main():
    parser = argparse.ArgumentParser(
        description='Implementation of Vigenere cipher')
    parser.add_argument('--lang', type=str, default='en', choices=['en', 'ru'],
        dest='language', help='language of the plaintext')
    subparsers = parser.add_subparsers()

    parser_encrypt = subparsers.add_parser(COMMAND_ENCRYPT, help='encrypt file')
    parser_encrypt.add_argument('--key', type=str, help='encryption key',
        required=True)
    add_file_arguments(parser_encrypt)
    parser_encrypt.set_defaults(func=encrypt)

    parser_decrypt = subparsers.add_parser(COMMAND_DECRYPT, help='decrypt file')
    parser_decrypt.add_argument('--key', type=str, help='decryption key')
    add_file_arguments(parser_decrypt)
    parser_decrypt.set_defaults(func=decrypt)

    parser_break = subparsers.add_parser(COMMAND_BREAK,
        help='break file encryption')
    parser_break.add_argument('--l', dest='kasiski_length', type=int,
        required=True, help='length of l-grams in Kasiski algorithm')
    add_file_arguments(parser_break)
    parser_break.set_defaults(func=break_encryption)
    args = parser.parse_args()
    args.func(args)

if __name__ == '__main__':
    main()
